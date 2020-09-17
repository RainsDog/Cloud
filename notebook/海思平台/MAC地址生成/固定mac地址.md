# MAC地址的组成

* mac地址都是由IEEE的注册管理机构RA分配给厂商，分配时只分配前三个字节，后三个字节由各厂商自行分配
* mac地址由IEEE组织统一管理，所有合法的mac地址都可以通过IEEE官网查询到厂商
* IEEE组织会定期更新oui文件，用于查询mac地址厂商
* oui文件包含了mac地址的注册信息，包含mac地址前缀（前三个字节）和公司名等

**oui文件下载地址**

 *http://standards.ieee.org/develop/regauth/oui/oui.txt*

 可以在浏览器中下载或通过wget 命令获取

# 海思如何固定MAC地址

* 海思的芯片，在默认情况下，每次开机MAC地址都不一样

* 为了让系统拥有固定的IP，而且与其它机器不相同，可以在第一次开机时，生成一个随机MAC地址，并保存到文件中，以后每次开机，都去读取该文件中的MAC地址，具体实现如下
* 将以下代码添加到系统的启动脚本（如rcS中），即可实现效果

```sh
macaddr=undefined
while read LINE
        do
                macaddr=$LINE
        done < /etc/macaddr
                    
#echo $macaddr
if [ "$macaddr" = "undefined" ];then
        macaddr=48:49:50:$(($RANDOM%100)):$(($RANDOM%100)):$(($RANDOM%100))
        echo $macaddr > /etc/macaddr
        chmod 766 /etc/macaddr
fi
ifconfig eth0 down
ifconfig eth0 hw ether $macaddr
ifconfig eth0 192.168.10.99 up
route add default gw 192.168.10.1
```

# 脚本执行过程说明

* 读取文件`/etc/macaddr`的值，如果该文件不存在，则`$macaddr`为默认值`undefined`

* 如果判断`$macaddr = undefined`，则认为是烧录根文件系统后第一次开机，将会随机生成以`48:49:50`开头，后3个字节为随机数的一个MAC地址
* 随机生成的mac地址，将会被写入文件`/etc/macaddr`中
* 如果读取到了mac地址，则不需要随机生成，直接使用读到的值即可
* 注意到后3个字节的生成机制，是十进制，并不是十六进制
* 其中前3个字节为IEEE的注册管理机构RA分配给厂商的，这里随便写了一个，保证不和已经分配的起冲突
* 前三个字节的第一个字节，末尾的两位一般为0，具体原因可以参考[这篇文章](https://blog.csdn.net/qianguozheng/article/details/38336277)

