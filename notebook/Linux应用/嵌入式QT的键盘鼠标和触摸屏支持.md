在不使用触摸屏的情况下，嵌入式QT对多个键盘鼠标，包括热拔插，都是支持的很好的

但是一旦使用了触摸屏，就会发现鼠标失效了，因为使用触摸屏，必然要使用tslib进行校准，否则屏幕的箭头是很不准的，使用了tslib，就必须在环境变量QWS_MOUSE_PROTO中指定触摸屏的设备节点，于是鼠标就不起作用了

关于如何实现同时支持触摸屏和鼠标，网上有很多做法，其实无非是两种，一个环境变量QWS_MOUSE_PROTO中即指定触摸屏节点，也指定鼠标节点，但是这种有非常大的问题，就是不一定每次生成设备节点的时候，都是按照你设想的顺序来的，比如之前tslib是even1，你插上鼠标，下次开机tslib就变成event2了，而且如果接了usb hub，那么设备节点就更难提前确定了，第二种是要改内核驱动，取消tslib的mouse节点上报，还有一些人改驱动将鼠标节点固定为eventM之类的

你会发现其实核心问题就是：环境变量QWS_MOUSE_PROTO怎么知道你设备对应的具体节点是多少

这里给出我的做法，使用脚本获取，如果你对shell脚本比较熟悉，那么稍微看一下，就应该能懂了

如果你看不懂，没关系，往下翻，我会从思路到实现简单的做一个分析

```sh
#!/bin/sh

QWS_MOUSE_PROTO=""
QWS_KEYBOARD=""
qtts=""
qtmouse=""
qtkbd=""
qttsfind=false
qtmousefind=false
qtkbdfind=false
qtwireless=false

cat /proc/bus/input/devices | sed 's/\"//g' | while read line
do
	# set touch screen
	if [ "$line" = "N: Name=ILITEK ILITEK-TP" ];then
		qttsfind=true
	fi
	if [ $qttsfind = "true" ];then
		handlerinfo=`echo $line | grep "Handlers"`;
		#echo $handlerinfo
		if [[ -n $handlerinfo ]];then
			qtts=`echo $handlerinfo | awk '{print $3}'`
			QWS_MOUSE_PROTO="$QWS_MOUSE_PROTO tslib:/dev/input/$qtts"
			echo $QWS_MOUSE_PROTO > qtmouse.tmp
			echo $qtts > qtts.tmp
			qttsfind=false
		fi
	fi

	# set usb mouse
	str="Name"
	result=$(echo $line | grep "${str}") 
	if [[ "$result" != "" ]];then
		str="Mouse"
		result=$(echo $line | grep "${str}")
		if [[ "$result" != "" ]];then
			qtmousefind=true
		fi
		str="MOUSE"
		result=$(echo $line | grep "${str}")
		if [[ "$result" != "" ]];then
			qtmousefind=true
		fi
		str="mouse"
		result=$(echo $line | grep "${str}")
		if [[ "$result" != "" ]];then
			qtmousefind=true
		fi 
	fi
	if [ $qtmousefind = "true" ];then
		handlerinfo=`echo $line | grep "Handlers"`;
		#echo $handlerinfo
		if [[ -n $handlerinfo ]];then
			qtmouse=`echo $handlerinfo | awk '{print $2}' | cut -c 10-15`
			qtmousefind=false
			QWS_MOUSE_PROTO="$QWS_MOUSE_PROTO mouseman:/dev/input/$qtmouse" 
			echo $QWS_MOUSE_PROTO > qtmouse.tmp
		fi
	fi
	
	# set keyboard
	str="Handlers"
	result=$(echo $line | grep "${str}")
	if [[ "$result" != "" ]];then
		str="sysrq"
		result=$(echo $line | grep "${str}")
		if [[ "$result" != "" ]];then
			qtkbd=`echo $result | awk '{print $4}'`
			QWS_KEYBOARD="$QWS_KEYBOARD linuxinput:/dev/input/$qtkbd"
			echo $QWS_KEYBOARD > qtkbd.tmp
		fi
	fi

	# set wireless receiver
	str="Name"
	result=$(echo $line | grep "${str}") 
	if [[ "$result" != "" ]];then
		str="Wireless"
		result=$(echo $line | grep "${str}")
		if [[ "$result" != "" ]];then
			qtwireless=true
		fi
		str="WIRELESS"
		result=$(echo $line | grep "${str}")
		if [[ "$result" != "" ]];then
			qtwireless=true
		fi
		str="wireless"
		result=$(echo $line | grep "${str}")
		if [[ "$result" != "" ]];then
			qtwireless=true
		fi
	fi
	if [ $qtwireless = "true" ];then
		handlerinfo=`echo $line | grep "Handlers"`;
		#echo $handlerinfo
		if [[ -n $handlerinfo ]];then
			qtwireless=false                            
			result=$(echo $line | grep "mouse")  
			kbd=$(echo $line | grep "sysrq")
			if ([ "$result" != "" ] && [ "$kbd" = "" ]);then
				qtmouse=`echo $handlerinfo | awk '{print $3}'`
				QWS_MOUSE_PROTO="$QWS_MOUSE_PROTO mouseman:/dev/input/$qtmouse"
				echo $QWS_MOUSE_PROTO > qtmouse.tmp
			fi
		fi
	fi

done

if [ -f "qtmouse.tmp" ];then
	QWS_MOUSE_PROTO=`cat qtmouse.tmp`
	rm qtmouse.tmp
fi
if [ -f "qtkbd.tmp" ];then
	QWS_KEYBOARD=`cat qtkbd.tmp`
	rm qtkbd.tmp
fi
if [ -f "qtts.tmp" ];then
	qtts=`cat qtts.tmp`
	rm qtts.tmp
fi

export TSLIB_ROOT=/home/root/tslib
export TSLIB_TSDEVICE=/dev/input/$qtts
export TSLIB_CONFFILE=$TSLIB_ROOT/etc/ts.conf
export TSLIB_PLUGINDIR=$TSLIB_ROOT/lib/ts
export TSLIB_CALIBFILE=$TSLIB_ROOT/etc/pointercal
export TSLIB_CONSOLEDEVICE=none
export TSLIB_FBDEVICE=/dev/fb1
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$TSLIB_ROOT/lib
export QWS_MOUSE_PROTO
export QWS_KEYBOARD
#echo $QWS_MOUSE_PROTO
#echo $QWS_KEYBOARD
#echo $TSLIB_TSDEVICE
```



**实现思路**

首先应该知道，所有的输入子系统的设备，都能通过这句话查看到具体信息

```sh
cat /proc/bus/input/devices
```

比如我这边，接了2个鼠标，1个键盘，加一套无线键鼠，打印信息就比较多了

```sh
I: Bus=0019 Vendor=0001 Product=0001 Version=0100
N: Name="gpio-keys"
P: Phys=gpio-keys/input0
S: Sysfs=/devices/platform/gpio-keys/input/input0
U: Uniq=
H: Handlers=kbd event0 
B: EV=3
B: KEY=2 0

I: Bus=0003 Vendor=222a Product=0001 Version=0110
N: Name="ILITEK ILITEK-TP"
P: Phys=usb-musb-hdrc.0-1/input0
S: Sysfs=/devices/platform/omap/ti81xx-usbss/musb-hdrc.0/usb1/1-1/1-1:1.0/input/input1
U: Uniq=
H: Handlers=mouse0 event1 
B: EV=1b
B: KEY=400 0 1f0000 0 0 0 0 0 0 0 0
B: ABS=fff00 3fffff
B: MSC=10

I: Bus=0003 Vendor=1c4f Product=0002 Version=0110
N: Name="SIGMACHIP USB Keyboard"
P: Phys=usb-musb-hdrc.1-1.3/input0
S: Sysfs=/devices/platform/omap/ti81xx-usbss/musb-hdrc.1/usb2/2-1/2-1.3/2-1.3:1.0/input/input5
U: Uniq=
H: Handlers=sysrq kbd event5 
B: EV=120013
B: KEY=10000 7 ff800000 7ff febeffdf f3cfffff ffffffff fffffffe
B: MSC=10
B: LED=7

I: Bus=0003 Vendor=1c4f Product=0002 Version=0110
N: Name="SIGMACHIP USB Keyboard"
P: Phys=usb-musb-hdrc.1-1.3/input1
S: Sysfs=/devices/platform/omap/ti81xx-usbss/musb-hdrc.1/usb2/2-1/2-1.3/2-1.3:1.1/input/input6
U: Uniq=
H: Handlers=kbd event6 
B: EV=1f
B: KEY=837fff 2c3027 bf004444 0 0 1 c04 b27c000 267bfa d941dfed 9e0000 0 0 0
B: REL=40
B: ABS=1 0
B: MSC=10

I: Bus=0003 Vendor=0000 Product=0538 Version=0111
N: Name=" USB OPTICAL MOUSE"
P: Phys=usb-musb-hdrc.1-1.4/input0
S: Sysfs=/devices/platform/omap/ti81xx-usbss/musb-hdrc.1/usb2/2-1/2-1.4/2-1.4:1.0/input/input7
U: Uniq=
H: Handlers=mouse3 event7 
B: EV=17
B: KEY=70000 0 0 0 0 0 0 0 0
B: REL=103
B: MSC=10

I: Bus=0003 Vendor=093a Product=2510 Version=0111
N: Name="PixArt USB Optical Mouse"
P: Phys=usb-musb-hdrc.1-1.1/input0
S: Sysfs=/devices/platform/omap/ti81xx-usbss/musb-hdrc.1/usb2/2-1/2-1.1/2-1.1:1.0/input/input8
U: Uniq=
H: Handlers=mouse1 event2 
B: EV=17
B: KEY=70000 0 0 0 0 0 0 0 0
B: REL=103
B: MSC=10

I: Bus=0003 Vendor=248a Product=8367 Version=0111
N: Name="Telink Wireless Receiver"
P: Phys=usb-musb-hdrc.1-1.2/input0
S: Sysfs=/devices/platform/omap/ti81xx-usbss/musb-hdrc.1/usb2/2-1/2-1.2/2-1.2:1.0/input/input9
U: Uniq=
H: Handlers=kbd mouse2 event3 
B: EV=1f
B: KEY=837fff 2c3027 bf004444 0 0 1f0001 f84 8b27c000 667bfa d941dfed 9e0000 0 0 0
B: REL=143
B: ABS=1 0
B: MSC=10

I: Bus=0003 Vendor=248a Product=8367 Version=0111
N: Name="Telink Wireless Receiver"
P: Phys=usb-musb-hdrc.1-1.2/input1
S: Sysfs=/devices/platform/omap/ti81xx-usbss/musb-hdrc.1/usb2/2-1/2-1.2/2-1.2:1.1/input/input10
U: Uniq=
H: Handlers=sysrq kbd event4 
B: EV=120013
B: KEY=10000 7 ff800000 7ff febeffdf ffefffff ffffffff fffffffe
B: MSC=10
B: LED=1f
```

我的思路是，通过Name字段，判断当前是什么设备，然后再查找Handlers字段，提取出设备节点

首先看触摸屏，触摸屏一般情况下，名字都是固定的，这个要根据实际情况来

我通过这几句实现的效果就是，进行多次循环，每次读取一行，同时将读取到的内容，利用正则表达式去除掉双引号，因为双引号不方便判断处理，也就是说将上述的64行逐行读出来，然后再操作

````sh
cat /proc/bus/input/devices | sed 's/\"//g' | while read line
do
...
done
````

接下来对触摸屏进行操作

```sh
# set touch screen
if [ "$line" = "N: Name=ILITEK ILITEK-TP" ];then
	qttsfind=true
fi
if [ $qttsfind = "true" ];then
	handlerinfo=`echo $line | grep "Handlers"`;
	#echo $handlerinfo
	if [[ -n $handlerinfo ]];then
        qtts=`echo $handlerinfo | awk '{print $3}'`
        QWS_MOUSE_PROTO="$QWS_MOUSE_PROTO tslib:/dev/input/$qtts"
        echo $QWS_MOUSE_PROTO > qtmouse.tmp
        echo $qtts > qtts.tmp
        qttsfind=false
	fi
fi
```

可以看出，如果找到名字等于我触摸屏的话，就将变量qttsfind为true，这里多提一句，shell里面没有布尔值的变量，我现在是把字符串变量当布尔值用而已。另外为什么我用了两个if，因为在一次循环中，只能取到name字段，在其后的几次循环中，才能取到Handlers字段，所以一旦name字段取到想要的值，就先做个标记，后面的循环中，先判断标记，然后判断是否是Handlers字段，然后通过awk取出具体的节点，至于为什么要重定位到一个文件中，是因为我发现里面给变量赋值了，到了外面居然没生效，本小白没想明白为什么，于是只能用这种笨方法，在外面读取文件，拿到对应的信息

```sh
if [ -f "qtmouse.tmp" ];then
	QWS_MOUSE_PROTO=`cat qtmouse.tmp`
	rm qtmouse.tmp
fi
export TSLIB_ROOT=/home/root/tslib
export TSLIB_TSDEVICE=/dev/input/$qtts
export TSLIB_CONFFILE=$TSLIB_ROOT/etc/ts.conf
export TSLIB_PLUGINDIR=$TSLIB_ROOT/lib/ts
export TSLIB_CALIBFILE=$TSLIB_ROOT/etc/pointercal
export TSLIB_CONSOLEDEVICE=none
export TSLIB_FBDEVICE=/dev/fb1
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$TSLIB_ROOT/lib
export QWS_MOUSE_PROTO
export QWS_KEYBOARD
#echo $QWS_MOUSE_PROTO
#echo $QWS_KEYBOARD
#echo $TSLIB_TSDEVICE
```

其实键盘鼠标的思路和触摸屏差不多，具体代码就不贴了，但是处理起来有一些区别，因此细节部分再提一下

比如说鼠标的名字字段，一定会有Mouse/mouse/MOUSE，不管是大小还是小写，因此都要进行判断，无线也是一样

然后就是鼠标的`H: Handlers=mouse3 event7`，发现mouse和Handlers是连体婴儿，因此要用cut把mouse切出来，但是这种方法导致的问题是，设备数量过过，比如超过10个，就会切错，但是我估计应该没人这么丧心病狂接这么多输入设备

然后就是键盘的`H: Handlers=sysrq kbd event5`，其实注意到键盘还有一个`H: Handlers=kbd event6`，但是实际起作用的是包含sysrq的那一个event，另外一个其实是没有数据上来的

再就是无线键鼠，名字部分带有`Wireless`，我不知道其它的无线键鼠是什么情况，因此跟鼠标一样，将大小写全部包括其中了，还有两个地方`H: Handlers=kbd mouse2 event3 `和`H: Handlers=sysrq kbd event4 `，其中sysrq所在的那个event是键盘的数据，mouse所在的那个是鼠标数据，我目前对键盘的判断，都是以sysrq为准，因此直接能找到键盘，但是鼠标是要判断名字是mouse的，因此找不到，这里添加了对名字是wireless的判断，单独增加无线鼠标的设备

另外QT环境变量中，同时指定多个设备，是用空格连起来的，不是用冒号，这点要注意

最后我的环境变量是，这就是上面脚本得到的结果

```sh
export QWS_MOUSE_PROTO="tslib:/dev/input/event1 mouseman:/dev/input/mouse3 mouseman:/dev/input/mouse1 mouseman:/dev/input/mouse2"
export QWS_KEYBOARD="linuxinput:/dev/input/event5 linuxinput:/dev/input/event4"
```

但是遗憾的是不支持热拔插，这个后面再说吧



