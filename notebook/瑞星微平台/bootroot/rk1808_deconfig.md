```shell
Target options  --->
Build options  ---> 
Toolchain  --->
System configuration  --->
Kernel  --->
Target packages  ---> 
Filesystem images  --->
Bootloaders  --->  
Host utilities  ---> 
Legacy config options  ---> 
```




```shell
Target options  --->
	Target Architecture (AArch64 (little endian))  --->	#64位小端
	Target Binary Format (ELF)  --->					#ELF格式二进制
	Target Architecture Variant (cortex-A35)  --->		#架构
	[ ] Enable NEON SIMD extension support				#不支持NEON？
	Floating point strategy (FP-ARMv8)  --->			#浮点策略？
```

```shell
Commands  --->
(...buildroot/configs/rockchip_rk1808_defconfig) Location to save buildroot config	#使用哪个配置文件
($(TOPDIR)/dl) Download dir								#下载目录，第三方程序源文件下载到哪（全是压缩包）
($(BASE_DIR)/host) Host dir								#主机目录
	Mirrors and Download locations  --->				#镜像和下载位置--->
(0) Number of jobs to run simultaneously (0 for auto)   #编译线程数-自动分配 
[ ] Enable compiler cache 								#编译器缓存，不知干啥的
[ ] build packages with debugging symbols 				#不需要带调试符号，否则文件老大了，占地方
[*] strip target binaries 								#据说会使文件尺寸变小
()    executables that should not be stripped  			
()    directories that should be skipped when stripping    
	gcc optimization level (optimize for size)  ---> 
[ ] Enable google-breakpad support  
	libraries (shared only)  ---> 
($(CONFIG_DIR)/local.mk) location of a package override file
()  global patch directories
	Advanced  ---> 
	*** Security Hardening Options ***
	Stack Smashing Protection (None)  --->
	RELRO Protection (None)  ---> 
	Buffer-overflow Detection (FORTIFY_SOURCE) (None)  --->
```



```shell
Mirrors and Download locations  --->
	 ()  Primary download site
	 (http://sources.buildroot.net) Backup download site
	 (https://cdn.kernel.org/pub) Kernel.org mirror 
	 (http://ftpmirror.gnu.org) GNU Software mirror
	 (http://rocks.moonscript.org) LuaRocks mirror
	 (http://cpan.metacpan.org) CPAN mirror (Perl packages)
#这些网站看起来都外国网站，下载速度应该是很慢的，其实可以改
```









