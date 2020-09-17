alsa官网下载alsa-lib-1.1.7.tar.bz2、alsa-utils-1.1.7.tar.bz2

版本不用太新，解压后，先编译alsa-lib

```shell
sudo ./configure --host=arm-linux  --prefix=/home/glx/share/hi3531D/glxtest/install  CC=arm-hisiv500-linux-gcc
```

指定host、安装路径、工具链即可，安装路径最好用绝对路径

configure没问题后，sudo make & sudo make install

这时安装路径已经多了一些文件



再编译utils

```shell
sudo ./configure --host=arm-linux  --prefix=/home/glx/share/hi3531D/glxtest/install  CC=arm-hisiv500-linux-gcc CFLAGS=-I/home/glx/share/hi3531D/glxtest/install/include LDFLAGS=-L/home/glx/share/hi3531D/glxtest/install/lib/  --disable-alsamixer --with-curses=ncurses --disable-xmlto --disable-nls    
```

头文件和库也用绝对路径来指定，然后sudo make & sudo make install



最后把生成的sbin、bin、lib都拷贝到机器的对应目录下

录制命令

```cpp
arecord -t wav -f dat -c 2 -d 10  -r 48000 ./record.wav
```



第一次执行arecord时报错如下

```shell
ALSA lib conf.c:3943:(snd_config_update_r) Cannot access file /home/glx/share/hi3531D/glxtest/install/share/alsa/alsa.conf
ALSA lib pcm.c:2565:(snd_pcm_open_noupdate) Unknown PCM hw:0,0
arecord: main:828: audio open error: No such file or directory
~ # 
~ # mkdir /home/glx/share/hi3531D/glxtest/install/share/alsa/
mkdir: can't create directory '/home/glx/share/hi3531D/glxtest/install/share/alsa/': No such file or directory
```



因此在本地对应的目录下，创建这样的目录，并把编译出来的东西都搬过来



第二次执行，报错，没有用户组

```cpp
ALSA lib pcm_direct.c:1943:(snd1_pcm_direct_parse_open_conf) The field ipc_gid must be a valid group (create group audio)
arecord: main:828: audio open error: Invalid argument
```

修改/etc/group

```shell
root::0:
admin:x:0:admin
audio:x:0:audio
anonymous:x:500:anonymous
```

