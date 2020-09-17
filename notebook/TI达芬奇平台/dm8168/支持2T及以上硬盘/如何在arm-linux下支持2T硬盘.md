[TOC]

# 前言

因产品需要，硬盘容量从1T提升到4T，当前的磁盘的分区表为mgr，文件系统为ext3

这两个选项均不支持2T以上硬盘，所以要对内核进行一些修改和调试，本文档记录调试过程中碰到的问题和解决办法



# 硬盘的分区

首先得将硬盘分区表改为GPT，由于arm linux上的工具比较简陋，在没有移植parted之前，暂时选择在ubuntu上做分区工作

ubuntu接上硬盘后，用fdisk查看，可以得到基本的信息，比如容量2000GB

```shell
sudo fdisk -l

Disk /dev/sda: 2000.4 GB, 2000398934016 bytes
255 heads, 63 sectors/track, 243201 cylinders, total 3907029168 sectors
Units = sectors of 1 * 512 = 512 bytes
Sector size (logical/physical): 512 bytes / 4096 bytes
I/O size (minimum/optimal): 4096 bytes / 4096 bytes
Disk identifier: 0x00000000
```

1. 设置分区表为GPT

   ```cpp
   /* # 表示root用户权限模式下，用命令行终端执行, 后文不再解释 */
   //sda这个参数与fdisk -l显示的硬盘节点对应，有可能是sdb或sdc，不要搞错了把自己硬盘格了
   //parted会进入自己程序的命令行界面，输入q或quit可以退出
   # parted /dev/sda
   mklabel gpt
   ```

2. 创建分区

   ```cpp
   //全部空间都给primary分区
   mkpart primary 0 100%
   ```

3. 格式化分区

    ```cpp
	//先退出parted
    quit
    //创建分区后，查看/dev/sda，可以发现多了一个/dev/sda1
    # mkfs.ext4 -F /dev/sda1
    ```
    
4. 挂载到pc上

   ```cpp
   //格式化完成后，可以尝试挂载到pc上，看能不能成功，注意挂载的目的路径一定要存在
   # mkdir /mnt/test
   # mount /dev/sda1 /mnt/test
   ```

5. 挂载成功后，应该能用 `df -h ` 查看了	

# 内核的修改

以下内容都需要选中

```cpp
Enable the block layer  --->
	[*]   Support for large (2TB+) block devices and files
	
File systems  ---> 
	<*> The Extended 4 (ext4) filesystem
 	[*]   Ext4 extended attributes
	[*]     Ext4 POSIX Access Control Lists
	[*]     Ext4 Security Labels
	
	Partition Types  --->
		[*]   EFI GUID Partition support
```



# 调试过程中遇到的其它问题

## 默认挂载失败

产品之前是用的1T硬盘，开机就会按照脚本，执行`mount -a`自动进行挂载，现在换了GPT格式的硬盘，默认挂载不起作用，反而报错了，其实这个问题很好解决，修改一下 `/etc/fstab` 文件即可

通过查看该文件，可以看到这么一句话

```cpp
/dev/sda      /zigsun       ext3      defaults       0  0
```

很明显这个挂载肯定会失败，改为如下即可

```cpp
/dev/sda1      /zigsun       ext4      defaults       0  0
```



## 驱动加载失败

报错如下：

```cpp
syslink: disagrees about version of symbol module_layout
osa_kermod: disagrees about version of symbol module_layout
```

由于内核重新编译过，这里是说版本上和驱动模块的ko文件冲突，这个解决起来稍微麻烦一点

解决这个有两种办法，一是ko重新编，二是内核重新编

我们知道海思的很多ko是不开源的，所以我们只能改内核，重新编的ko加载后发现报了更多的错，而且网上还查不到

```shell
*** DM8168VPSSM3PROC_map: AddrTable_SIZE reached!
Error [0x86a85003] at Line no: 1778 in file /zigsun/8003/DVRRDK_04.00.00.03/ti_tools/syslink/syslink_2_21_03_11/packages/ti/syslink/utils/hlos/knl/Linux/../../../../../../ti/syslink/family/common/ti81xx/ti81xxducati/ti81xxvpssm3/Dm8168M3DssProc.c
```

所以改内核就完事了

首先取消内核的版本检测

```shell
[*] Enable loadable module support  --->
	[ ]   Module versioning support 	# 这一项要关闭
```

改完后，加载还是报错

```shell
syslink: version magic '2.6.37-2.0.0 mod_unload modversions ARMv7 p2v8 ' should be '2.6.37-2.0.0 mod_unload ARMv7 p2v8 '
```

虽然内核不检查了，但是ko还在对比这个版本，这两个version magic明显不一致，前面的多一个modversions，首先要搞清楚，哪边是内核的version magic，哪边是驱动的version magic

先看一下ko的信息

```shell
cd opt/dvr_rdk/ti816x/kermod/ 
modinfo syslink.ko
```

得到

```shell
vermagic:       2.6.37-2.0.0 mod_unload modversions ARMv7 p2v8 
```

这么说内核应该是因为刚才的修改，少了modversions，因此想办法加上去就好了

```cpp
# vi include/linux/vermagic.h
```

改这个文件，不管条件成不成立，都加modversions上去

```shell
#ifdef CONFIG_MODVERSIONS
#define MODULE_VERMAGIC_MODVERSIONS "modversions "
#else
#define MODULE_VERMAGIC_MODVERSIONS "modversions " 
#endif
```

重新编译内核，重新烧录，发现已经能正常加载驱动模块了

## 其它报错

```shell
modprobe: FATAL: Could not load /lib/modules/2.6.37-2.0.0/modules.dep: No such file or directory
```

如果去找的话，发现本地根本没有 `2.6.37-2.0.0` 这个目录，也没有modules.dep文件

因此去创建一个这样的目录，再创建一个这样的文件，并给它读写权限，就不会报错了，但是这个影响不大



编译TI8168的这个DVRRDK时，还可能遇到一些报错

分析build_lsp.mk可得知，编译lsp时会将ti8168_etv_defconfig作为默认配置进行编译，因此之前要将修改过的config文件覆盖默认的ti8168_etv_defconfig

```shell
cd /zigsun/8003/DVRRDK_04.00.00.03/ti_tools/linux-dvr-rdk
cp .config arch/arm/configs/ti8168_etv_defconfig

cd /zigsun/8003/DVRRDK_04.00.00.03/dvr_rdk
./run_make.sh  ti816x-etv-hdmi sys
```

sys的依赖如下：

```shell
 sys:  uboot linux_deps syslink dvr_rdk
```

uboot就不用说了，linux_deps主要是编译内核，以及驱动模块，syslink也是编译一些ko，dvr_rdk是编译固件，其中编译lsp的modules时，仍然有报错

```shell
make[6]: *** 没有规则可以创建“/zigsun/8003/DVRRDK_04.00.00.03/ti_tools/linux-dvr-rdk/usr/include/linux/netfilter/.install”需要的目标“/zigsun/8003/DVRRDK_04.00.00.03/ti_tools/linux-dvr-rdk/include/linux/netfilter/xt_connmark.h”。 停止

make[6]: *** 没有规则可以创建“/zigsun/8003/DVRRDK_04.00.00.03/ti_tools/linux-dvr-rdk/usr/include/linux/netfilter/.install”需要的目标“/zigsun/8003/DVRRDK_04.00.00.03/ti_tools/linux-dvr-rdk/include/linux/netfilter/xt_dscp.h”。 停止。
```

经过排查，这些文件都有，但是都是大写的，全部复制一份改为小写就好了

例如，复制一份xt_CONNMARK.h改为小写的xt_connmark.h

有时候也会因为大写的没有而报错，这时就把小写的复制一份，名字改为大写的就好



```shell
#!/bin/bash
# 新建/dev/sdb的磁盘标签类型为GPT
parted /dev/sda mklabel gpt 
# 将/dev/sdb整个空间分给同一个分区
parted /dev/sda mkpart primary 0 100%
# 忽略警告
ignore
# 格式化分区
mkfs -t ext4 /dev/sda1
# 设定分区label(非必要)
e2label /dev/sdb1 /gfsdata01
# 创建挂载目录
mkdir /gfsdata01
# 临时挂载
mount /dev/sdb1 /gfsdata01
# 开机自动挂载(永久挂载)
echo '/dev/sdb1 /gfsdata01       ext4    defaults        0 0'>>/etc/fstab
```

