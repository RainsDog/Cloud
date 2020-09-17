```shell
cd rk1808_linux_200628/kernel
sudo make menuconfig
```
``` shell
[*] Networking support  ---> 
    Networking options  --->
        [*]   IP: kernel level autoconfiguration
        [*]     IP: DHCP support
        [*]     IP: BOOTP support
        [ ]     IP: RARP support
        
        
File systems  --->
    [*] Network File Systems  ---> 
        <*>   NFS client support  
        <*>     NFS client support for NFS version 2
        <*>     NFS client support for NFS version 3
        [*]       NFS client support for the NFSv3 ACL protocol extension
        <*>     NFS client support for NFS version 4
        [*]     Provide swap over NFS support 
        [*]   NFS client support for NFSv4.1
        [*]     NFS client support for NFSv4.2
        (kernel.org) NFSv4.1 Implementation ID Domain
        [*]     NFSv4.1 client support for migration
        [*]   Root file system on NFS
        [ ]   Use the legacy NFS DNS resolver
        < >   NFS server support
        [ ]   RPC: Enable dprintk debugging
        < >   Ceph distributed file system
        < >   CIFS support (advanced network filesystem, SMBFS successor)
        < >   NCP file system support (to mount NetWare volumes)
        < >   Coda file system support (advanced network fs)
        < >   Andrew File System support (AFS)
```

保存退出，保存默认配置文件

```cpp
sudo make savedefconfig
sudo cp defconfig ./arch/arm64/configs/rk1808_linux_defconfig 
```

修改设备树

```shell
vi kernel/arch/arm64/boot/dts/rockchip/rk1808-evb-v10.dts

# 使能gmac
&gmac {
	status = "okay";
};
```

编译烧录

```cpp
sudo ./build.sh kernel
```

