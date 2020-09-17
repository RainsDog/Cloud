# 前言

为了更深入了解rk1808 sdk，对其中关于uboot部分的脚本和makefile等做一个分析

主要参考文件：**Rockchip_Developer_Guide_Linux_Software_CN.pdf**

# uboot的编译

根据文档的说法，编译uboot有两种方法

* 在uboot目录下编译

这种方法，先cd到uboot目录下，然后执行

```shell
sudo ./make.sh rk1808
```

* 在顶层目录下编译

这种方法，利用底层目录的build.sh，传递参数uboot进行编译

```shell
sudo ./build.sh uboot
```

其实利用build.sh脚本，实际上也是cd到uboot目录下，去调用make.sh脚本进行编译，因此这两种方式其实可以认为是一种，那要想弄清楚来龙去脉，就得先分析build.sh脚本

# build.sh分析

直接跳到末尾，有这样一段话，首先判断是不是需要打印usage，如果不是，然后再对输入的参数，也就是`$@`进行遍历，并判断它是什么

例如我这样输入`sudo ./build.sh uboot`，在下面的case分支语句中，就会进入`eval build_$option || usage`，这个意思是`||`前能执行，就去执行，不能执行，就打印usage

```shell
#=========================
# build targets
#=========================

if echo $@|grep -wqE "help|-h"; then
        usage
        exit 0
fi

OPTIONS="$@"
for option in ${OPTIONS:-allsave}; do
	echo "processing option: $option"
	case $option in
		BoardConfig*.mk)
            option=$TOP_DIR/device/rockchip/$RK_TARGET_PRODUCT/$option
            ;&  
        *.mk)
            CONF=$(realpath $option)
            echo "switching to board: $CONF"
            if [ ! -f $CONF ]; then
                echo "not exist!"
                exit 1
            fi  
			ln -sf $CONF $BOARD_CONFIG
			;;  
		buildroot|debian|distro|yocto)
            build_rootfs $option
            ;;  
		recovery)
            build_kernel
            ;&  
        *)  
            eval build_$option || usage
            ;;  
	esac
done 
```

再往上看，可以找到编译uboot的函数，它首先删除了一个loader什么的bin文件，据说是用于初始化ddr的，这个暂时没太想明白

然后cd到uboot的目录下，执行`./make.sh rk1808`，它这个参数`$RK_UBOOT_DEFCONFIG`通过打印可以很清楚地知道，其实就是rk1808，然后判断上一个命令的退出状态，来确定执行成功与否

接下来就是分析make.sh了

```shell
function build_uboot(){
        echo "============Start build uboot============"
        echo "TARGET_UBOOT_CONFIG=$RK_UBOOT_DEFCONFIG"
        echo "========================================="
        if [ -f u-boot/*_loader_*.bin ]; then
                rm u-boot/*_loader_*.bin
        fi
        cd u-boot && ./make.sh $RK_UBOOT_DEFCONFIG && cd -
        if [ $? -eq 0 ]; then
                echo "====Build uboot ok!===="
        else
                echo "====Build uboot failed!===="
                exit 1
        fi
}
```

# make.sh

这个脚本比刚才的复杂的多，因为瑞星微的sdk是针对所有产品的，一份sdk可以编译出多款芯片的镜像文件，所以这个makefile、编译脚本等都很复杂，但是我们只要抽丝剥茧，抓住核心即可

还是老规矩，直接跳到末尾，看它调用了哪些函数

```shell
prepare
select_toolchain
select_chip_info
fixup_platform_configure
sub_commands
make CROSS_COMPILE=${TOOLCHAIN_GCC}  all --jobs=${JOB} ${OUTOPT}
pack_uboot_image
pack_loader_image
pack_trust_image
finish     
```

## prepare

```shell
prepare()
{
	local absolute_path cmd dir count

	# Parse output directory 'O=<dir>'
	cmd=${OUTDIR%=*}
	`if [ "${cmd}" = 'O' ]; then`
        OUTDIR=${OUTDIR#*=}
        OUTOPT=O=${OUTDIR}
	else
		case $BOARD in
            # Parse from exit .config
            ''|elf*|loader*|spl*|itb|debug*|trust|uboot|map|sym)
                count=`find -name .config | wc -l`
                dir=`find -name .config`
                # Good, find only one .config
                if [ $count -eq 1 ]; then
                    dir=${dir%/*}
                    OUTDIR=${dir#*/}
                    # Set OUTOPT if not current directory
                    if [ $OUTDIR != '.' ]; then
                        OUTOPT=O=${OUTDIR}
                    fi
                elif [ $count -eq 0 ]; then
                    echo
                    echo "Build failed, Can't find .config"
                    help
                    exit 1
                else
                    echo
                    echo "Build failed, find $count '.config': "
                    echo "$dir"
                    echo "Please leave only one of them"
                    exit 1
                fi
                ;;
            *)
                OUTDIR=.
                ;;
		esac
	fi

    # Parse help and make defconfig
    case $BOARD in
    	#Help
    	--help|-help|help|--h|-h)
   	 		help
    		exit 0
			;;

    	#Subcmd
        ''|elf*|loader*|spl*|itb|debug*|trust*|uboot|map|sym)
            ;;

		*)
            #Func address is valid ?
            if [ -z $(echo ${FUNCADDR} | sed 's/[0-9,a-f,A-F,x,X,-]//g') ]; then
            	return
            elif [ ! -f configs/${BOARD}_defconfig ]; then
                echo
                echo "Can't find: configs/${BOARD}_defconfig"
                echo
                echo "******** Rockchip Support List *************"
                echo "${SUPPORT_LIST}"
                echo "********************************************"
                echo
                exit 1
            else
            	echo "make for ${BOARD}_defconfig by -j${JOB}"
           	 	make ${BOARD}_defconfig ${OUTOPT}
            fi
			;;
	esac
	
    # Initialize RKBIN
    if [ -d ${RKBIN_TOOLS} ]; then
    	absolute_path=$(cd `dirname ${RKBIN_TOOLS}`; pwd)
    	RKBIN=${absolute_path}
    else
        echo
        echo "Can't find '../rkbin/' repository, please download it before pack image!"
        echo "How to obtain? 3 ways:"
        echo "  1. Login your Rockchip gerrit account: \"Projects\" -> \"List\" -> search \"rk/rkbin\" repository"
        echo "  2. Github repository: https://github.com/rockchip-linux/rkbin"
        echo "  3. Download full release SDK repository"
        exit 1
    fi
}
```

这个函数很长，首先出现了几个变量，然后先看分析第一段`# Parse output directory 'O=<dir>'`

```shell
BOARD=$1
OUTDIR=$2
OUTOPT=
```
我们传进来的`$1`是rk1808，后面两个OUT都是空，因此，不会进入`if [ "${cmd}" = 'O' ]; then`这个条件里面

进到第二个条件后，会去遍历我们传进来的参数 `$1`，也就是`BOARD`，然后第一个case条件是`''|elf*|loader*|spl*|itb|debug*|trust|uboot|map|sym)`很明显rk1808不满足这个，所以会执行末尾的`*)`，这个条件里面，只做了一件事，那就是`OUTDIR=.`

<hr/>

然后分析第二段，`# Parse help and make defconfig`

可以看到注释的help和subcmd仍然是不会被执行的，所以直接看`Func address is valid ?`部分

这个if条件的结果，是`rk`，不为空，所以不会return

然后判断`configs/rk1808_defconfig`这个默认配置文件在不在

最后唯一有用的部分，就是执行`make ${BOARD}_defconfig ${OUTOPT}`，也就是`make rk1808_defconfig .`

这句话的作用是从configs中拷贝配置文件到当前目录下

```shell
case $BOARD in                                                                                                                                            
        #Help
        --help|-help|help|--h|-h)
        help
        exit 0
        ;;  

        #Subcmd
        ''|elf*|loader*|spl*|itb|debug*|trust*|uboot|map|sym)
        ;;  

		*)  
        #Func address is valid ?
        if [ -z $(echo ${FUNCADDR} | sed 's/[0-9,a-f,A-F,x,X,-]//g') ]; then
                return
        elif [ ! -f configs/${BOARD}_defconfig ]; then
                echo
                echo "Can't find: configs/${BOARD}_defconfig"
                echo
                echo "******** Rockchip Support List *************"
                echo "${SUPPORT_LIST}"
                echo "********************************************"
                echo
                exit 1
        else
                echo "make for ${BOARD}_defconfig by -j${JOB}"
                make ${BOARD}_defconfig ${OUTOPT}
        fi  
        ;;  
esac
```

<hr/>

然后分析第三段` # Initialize RKBIN`

在uboot的同级目录下，有个rkbin目录，编译uboot时需要用到这里面的bin文件，这里就是为了将它的绝对路径赋值给RKBIN

```shell
# Initialize RKBIN                                                                                                                                        
if [ -d ${RKBIN_TOOLS} ]; then 
        absolute_path=$(cd `dirname ${RKBIN_TOOLS}`; pwd)
        RKBIN=${absolute_path}
else    
        echo    
        echo "Can't find '../rkbin/' repository, please download it before pack image!"
        echo "How to obtain? 3 ways:"
        echo "  1. Login your Rockchip gerrit account: \"Projects\" -> \"List\" -> search \"rk/rkbin\" repository"
        echo "  2. Github repository: https://github.com/rockchip-linux/rkbin"
        echo "  3. Download full release SDK repository"
        exit 1  
fi
```

## select_toolchain

根据当前目录下的`.config`文件，判断是否有`CONFIG_ARM64=y`，然后给TOOLCHAIN_GCC等变量赋一个绝对路径

```shell
select_toolchain()
{
    local absolute_path

    if grep  -q '^CONFIG_ARM64=y' ${OUTDIR}/.config ; then
        if [ -d ${TOOLCHAIN_ARM64} ]; then
            absolute_path=$(cd `dirname ${TOOLCHAIN_ARM64}`; pwd)
            TOOLCHAIN_GCC=${absolute_path}/bin/${GCC_ARM64}
            TOOLCHAIN_OBJDUMP=${absolute_path}/bin/${OBJ_ARM64}
            TOOLCHAIN_ADDR2LINE=${absolute_path}/bin/${ADDR2LINE_ARM64}
        else
            echo "Can't find toolchain: ${TOOLCHAIN_ARM64}"
            exit 1
        fi  
    else
        if [ -d ${TOOLCHAIN_ARM32} ]; then
            absolute_path=$(cd `dirname ${TOOLCHAIN_ARM32}`; pwd)
            TOOLCHAIN_GCC=${absolute_path}/bin/${GCC_ARM32}
            TOOLCHAIN_OBJDUMP=${absolute_path}/bin/${OBJ_ARM32}
            TOOLCHAIN_ADDR2LINE=${absolute_path}/bin/${ADDR2LINE_ARM32}
        else
            echo "Can't find toolchain: ${TOOLCHAIN_ARM32}"
            exit 1
        fi  
    fi  

    # echo "toolchain: ${TOOLCHAIN_GCC}"
}
```

## select_chip_info

这个函数个人感觉不怎么重要，所以代码就不贴了，它的作用也是从.config读取一些信息，然后给以下变量赋值

RKCHIP_LABEL、RKCHIP_LOADER、RKCHIP_TRUST



## fixup_platform_configure

这个函数也没什么意思，也是在赋值

```shell
PLATFORM_RSA="--rsa 3"
PLATFORM_UBOOT_IMG_SIZE="--size 1024 2"
PLATFORM_TRUST_IMG_SIZE="--size 1024 2"
```



## sub_commands

说实话我没看懂他要干啥



## make

```
make CROSS_COMPILE=${TOOLCHAIN_GCC}  all --jobs=${JOB} ${OUTOPT}
```

这个其实就没啥好说的了，那个jobs是编译所用的线程数，output应该是生成文件的路径，默认是空的

最终是生成`u-boot.img` `u-boot-dtb.img` `u-boot.bin` `u-boot-dtb.bin`等文件

## pack_uboot_image

打包uboot前，先检查`u-boot.bin`的大小，超过10M，就报错了

```shell
# Check file size
    UBOOT_KB=`ls -l u-boot.bin | awk '{print $5}'`
    if [ "$PLATFORM_UBOOT_IMG_SIZE" = "" ]; then
        UBOOT_MAX_KB=1046528
    else
        UBOOT_MAX_KB=`echo $PLATFORM_UBOOT_IMG_SIZE | awk '{print strtonum($2)}'`
        UBOOT_MAX_KB=$(((UBOOT_MAX_KB-HEAD_KB)*1024))
    fi  

    if [ $UBOOT_KB -gt $UBOOT_MAX_KB ]; then
        echo    
        echo "ERROR: pack uboot failed! u-boot.bin actual: $UBOOT_KB bytes, max limit: $UBOOT_MAX_KB bytes"
        exit 1  
    fi
```

最终在当前目录下，生成了uboot.img，并删除了其它的img结尾的中间文件

```shell
# Pack image
    UBOOT_LOAD_ADDR=`sed -n "/CONFIG_SYS_TEXT_BASE=/s/CONFIG_SYS_TEXT_BASE=//p" ${OUTDIR}/include/autoconf.mk|tr -d '\r'`
    if [ ! $UBOOT_LOAD_ADDR ]; then
        UBOOT_LOAD_ADDR=`sed -n "/CONFIG_SYS_TEXT_BASE=/s/CONFIG_SYS_TEXT_BASE=//p" ${OUTDIR}/.config|tr -d '\r'`
    fi

    ${RKTOOLS}/loaderimage --pack --uboot ${OUTDIR}/u-boot.bin uboot.img ${UBOOT_LOAD_ADDR} ${PLATFORM_UBOOT_IMG_SIZE}                                                        

    # Delete u-boot.img and u-boot-dtb.img, which makes users not be confused with final uboot.img
    if [ -f ${OUTDIR}/u-boot.img ]; then
        rm ${OUTDIR}/u-boot.img
    fi

    if [ -f ${OUTDIR}/u-boot-dtb.img ]; then
        rm ${OUTDIR}/u-boot-dtb.img
    fi
    echo "pack uboot okay! Input: ${OUTDIR}/u-boot.bin"
```

## pack_loader_image

这一步最终生成这个文件`rk1808_loader_v1.04.105.bin`，并拷贝到当前目录下

## pack_trust_image

这一步生成`trust.img`，这是arm搞的一个跟安全有关的东西`Trustzone`，大致是为了保护内存中的数据不被别人窃取之类的

## finish

最后一步就是打印一点信息，就ok了


# 总结

从结果来看，执行make.sh一共得到了以下几个东西

* uboot.img
* rk1808_loader_v1.04.105.bin
* trust.img

我们烧录的时候，是在顶层目录的rockdev去取镜像文件，在linux下，可以很清晰得看到链接的关系，所生成的这三个文件，都会被链接到

```shell
boot.img -> ../kernel/boot.img
MiniLoaderAll.bin -> ../u-boot/rk1808_loader_v1.04.105.bin
misc.img -> ../device/rockchip/rockimg/wipe_all-misc.img
oem.img
parameter.txt -> ../device/rockchip/rk1808/parameter-buildroot.txt
recovery.img -> ../buildroot/output/rockchip_rk1808_recovery/images/recovery.img
rootfs.ext4 -> ../buildroot/output/rockchip_rk1808/images/rootfs.ext2
rootfs.img -> ../buildroot/output/rockchip_rk1808/images/rootfs.ext2
trust.img -> ../u-boot/trust.img
uboot.img -> ../u-boot/uboot.img
update.img
userdata.img
```

除此之外，如果希望自己修改一些uboot的内容，可以通过类似内核的make menuconfig取修改.config文件，不过注意修改完了，一定要记得将该文件写回原来的位置，就是那个默认的deconfig所在的位置，否则每次编译，当前目录下的.config都会被configs目录下的xxx_deconfig覆盖掉

