# 前言

* 文本记录海思hi3531d在uboot的命令行，以及启动函数中，加载logo的过程

* 参考资料：HiMPP 开机画面使用指南.pdf

# 图片的要求

要求是jpeg、jpg格式，（帧头是0xFF 0xD8），不然无法解码，如果你自己用ps导的图片，出现无法解码，可以尝试在一些在线的图片格式转换网站，把图片转一下格式

# 命令行显示logo

1. 进入uboot后，不启动内核，进入命令行模式

2. 将图片通过tftp，或者从flash加载到内存中

* tftp方式：
```shell
tftp 0x43000000 logo.jpeg
```
* 读flash方式：
```shell
nand read 0x43000000 0x100000 0x26904
```

在命令行方式下，这些地址是多少并不重要，只要不影响到内存中的uboot程序以及寄存器地址即可，也就是说地址可以和官方文档中给的地址不一样，比如我想通过tftp，将logo下载到内存地址0x50000000，可不可以，当然是可以的，但是你要下载到0x30000000，那就真不行，因为那是寄存器映射的位置；另外就是读flash的有关参数，第一个地址是内存地址，第二个是flash地址，第三个是要读的长度，正常情况下，都是通过读flash去获取logo，所以在分区的时候，一定要增加对应的logo分区，由于此处只支持jpeg的解压，所以容量不需要很大，1M的flash空间是完全足够的（不像bmp图像，占很大的内存）

3. 设置环境变量
```shell
   setenv jpeg_addr 0x43000000
   setenv vobuf 0x43800000
   setenv jpeg_size 0x26904
```
在命令行方式下，此处的地址值也是可以设的比较随意，但是注意不要把内存中的uboot程序干掉了，这几个环境变量是一定需要的，因为解码要用到它们，需要注意的是，这个图像的size，不是一成不变的，需要根据你实际的大小，比如tftp下载完成后，就会告诉你大小，如此处的26904

```shell
zigsun # tftp 0x43000000 logo.jpeg
RTL8370 init start! 
ETH0: PHY(phyaddr=1, rgmii) link UP: DUPLEX=FULL : SPEED=1000M
MAC:   00-00-00-23-34-45
TFTP from server 192.168.10.101; our IP address is 192.168.10.100
Download Filename 'logo.jpeg'.
Download to address: 0x43000000
Downloading: #  [ Connected ]
####
done
Bytes transferred = 157956 (26904 hex)
```

4. JPEG解码

```shell
decjpg
```

这个解码用到的参数，来自于上一步设置的环境变量

5. 打开VO

```shell
startvo 0 36 12
```

0表示输出设备是hd0， 36表示vga & hdmi， 12表示1920*1080 60帧，这里的参数，官方文档还算写的详细，不清楚可以去看

6.  输出图像

```shell
startvl 0 0x43800000 1920 0 0  1920  1080
```

0表示输出设备是hd0，0x43800000表示解码后，数据存放的地址，1920这个参数是**stride**，要求64字节对齐，一般就取图像的长度，最后四个参数是坐标和大小，这个没什么好说的

# 环境变量法显示logo

本方法的核心思想，是将上述的命令行操作，放到环境变量里面，例如bootcmd

```cpp
#define CONFIG_BOOTCOMMAND	"setenv jpeg_addr 0x43000000;\
							setenv jpeg_size 0x26904;\
							setenv vobuf 0x43800000;\
							nand read 0x43000000 0x100000 0x26904;\
							decjpg;\
							startvo 0 36 12;\
							startvl 0 0x43800000 1920 0 0  1920  1080;\
							nand read 0x42000000 0x600000 0x800000;bootm 0x42000000"


setenv bootcmd 'setenv jpeg_addr 0x43000000;setenv jpeg_size 0x26904;setenv vobuf 0x43800000;nand read 0x43000000 0x100000 0x26904;decjpg;startvo 0 36 12;startvl 0 0x43800000 1920 0 0  1920  1080;nand read 0x42000000 0x600000 0x800000;bootm 0x42000000'
```

前者是在uboot的对应芯片的配置文件中，修改默认的环境变量，后者是通过命令行，去设置bootcmd环境变量

可以看到这会导致该环境变量很长，命令也都是之前见过的，所以操作起来还算容易

# 函数法显示logo

此方法，是将海思提供的API，在do_bootm函数中完成调用

```shell
bootcmd=nand read 0x42000000 0x600000 0x800000;bootm 0x42000000
```

可以看到，先是将内核从flash读到内存地址0x42000000，而且指定了从flash的哪开始读，读多少字节，然后bootm启动内核，这里要启动logo，就是要在do_bootm中添加启动logo的代码

这个方法需要注意一些问题，例如，我现在是将kernel加载到内存地址0x42000000，然后程序跳转到这个地址

之前的图片都是加载到0x43000000，然后解压后放到0x43800000，如果内核很大呢，超过16M，那内核一定会占用图片所在的内存，而图片是后加载的，也就是说，内核会被图片破坏掉

但是内核超过16M，这中情况，应该是不会遇到吧，我就不信有人编译一个内核出来16M，那还叫嵌入式吗？？



需要注意的是，海思官方文档提供的API，并不能完成LOGO的加载和显示，具体怎么实现的，还要参考使用命令行时，具体的函数调用过程



## 环境变量的添加

打开芯片的配置文件`u-boot-2010.06\include\configs\hi3531d.h`，添加以下宏定义

```cpp
/*-----------------------------------------------------------------------
 * uboot logo
 *-----------------------------------------------------------------------*/
#define CONFIG_EXTRA_ENV_SETTINGS
#define CONFIG_JPEG_ADDR 	"0x43000000" 
#define CONFIG_JPEG_SIZE 	"0x26904" 		
#define CONFIG_VOBUF 		"0x43800000"
```

可以看到基本上还是之前说过的一些参数



打开这个文件`u-boot-2010.06\common\env_common.c`，增加新的环境变量，这样以后这些参数可以在命令行使用setenv进行更改

```cpp
#ifdef  CONFIG_EXTRA_ENV_SETTINGS
	//CONFIG_EXTRA_ENV_SETTINGS
	"jpeg_addr=" 	CONFIG_JPEG_ADDR 	"\0"
	"jpeg_size=" 	CONFIG_JPEG_SIZE 	"\0"
	"vobuf=" 	 	CONFIG_VOBUF 		"\0"
#endif
```



## 修改图片加载函数

打开文件`u-boot-2010.06\product\hiosd\dec\jpegd.c`

增加一个全局变量，用以记录从环境变量中读到的图片地址

```cpp
unsigned long jpeg_addr=0;
```

修改一下load_jpeg函数

```cpp
int load_jpeg(void)
{
...
    hilogo = (unsigned char *)u;
	jpeg_addr = u;
    /*if (*(volatile unsigned char *)hilogo != 0xFF || *(volatile unsigned char *)(hilogo+1) != 0xD8)
    {
		printf("addr:%#x,size:%ld,logoaddr:%#lx,:%2x,%2x\n",(unsigned int)(void *)hilogo,jpeg_size,u
        		,*(volatile unsigned char *)hilogo,*(volatile unsigned char *)(hilogo+1));
            return -1;
     }*/
...
}
```

这个函数有个什么问题呢，就是它会检查图片的开头是不是0xFF 0xD8，只要不符合要求，就直接返回-1，这导致很多图片虽然是jpg格式，分辨率也没问题，但是解码失败，因此很简单粗暴直接注释掉，你不要管我图片怎么样，先去解码行不行

然后还要记录从环境变量中读到的图片地址，jpeg_addr

## 修改bootm

打开`u-boot-2010.06\common\cmd_bootm.c`，找到`do_bootm`函数

增加一个局部变量，用来从flash中读取图片

```cpp
char cmd[64];
```

在该函数的开头，增加这样一段，即可大功告成

```cpp
#ifdef CONFIG_EXTRA_ENV_SETTINGS
	extern int load_jpeg(void);
	extern int jpeg_decode(void);
	extern int start_vo(unsigned int dev, unsigned int type, unsigned int sync);
	extern int hdmi_display(unsigned int vosync, unsigned int input, unsigned int output);	  
	extern int start_videolayer(unsigned int layer, unsigned addr, unsigned int strd,
        unsigned int x, unsigned int y, unsigned int w, unsigned int h);
	extern unsigned long jpeg_size;
	extern unsigned long jpeg_addr;

	extern unsigned long VIDEO_DATA_BASE;
	load_jpeg();			//read para form ENV
	sprintf(cmd, "nand read %x 0x100000 %x", jpeg_addr, jpeg_size);
	printf("read jpeg form flash, cmd = %s\n",cmd);
	run_command(cmd,0);		//read JPEG form flash
	jpeg_decode();			//JPEG decode
	start_vo(0, 36, 12); 	
    hdmi_display(12, 2, 2);	//display
	start_videolayer(0, VIDEO_DATA_BASE, 1920, 0, 0, 1920, 1080);
#endif
```

有人可能会问，这个宏定义在哪打开的，这个是之前在芯片配置文件中添加的，可以往上翻翻，是说了的

然后为什么要对变量和函数进行extern声明，因为会报错啊

然后这里有一个run_command的调用，就是从flash加载图片到内存中

然后这些加载的步骤，也没什么好说的，是根据用命令行执行时函数的调用顺序，抄过来的



# 单独增加bootlogo命令

这种方法，也没什么好说的，就是增加一个文件`cmd_bootlogo.c`，实现bootlogo命令，在uboot中有很多这种文件，例如`cmd_boot.c cmd_ao.c cmd_bedbug.c`等等，仿照它们写就完事了吗，然后代码，可以参考上一个方法

最后，在环境变量bootcmd中，设置如下

```shell
bootcmd=bootlogo;nand read 0x42000000 0x600000 0x800000;bootm 0x42000000
```

先调用命令去显示logo，然后去启动内核

