#/bin/sh
set -x

cmd=$1
srcDir=`pwd`/../../..
targetDir=/zigsun/bin/linux/bin.debug.Linux.Arm
hwBoardType=BOARD_V1.3
#hwBoardType=BOARD_V2.0
hwBoard2V0mtd3addr=880000

sdSrcDir=/mnt/nfs/zigsun
sdTargetDir=/media/sda1

echo ${srcDir}
echo ${targetDir}
#busybox mount -o nolock 192.168.1.141:/zigsun /mnt/nfs/zigsun
getkernelstartaddr()
{
	mtd0addr=`cat /proc/mtd | grep mtd0 | cut -d " " -f 2`
	mtd1addr=`cat /proc/mtd | grep mtd1 | cut -d " " -f 2`
	mtd2addr=`cat /proc/mtd | grep mtd2 | cut -d " " -f 2`
	hwBoard2v0mtd3addr=`printf "%x" $((16#$mtd0addr + 16#$mtd1addr + 16#$mtd2addr))` 
	echo ${hwBoard2v0mtd3addr}
}
getHwBoardType()
{
	Kernel1V3=`dmesg | grep rt5640`
	Kernel1V3OnBoard2V0=`dmesg | grep rt5640 | grep Fail`
	Kernel2V0OnBoard1V3=`dmesg | grep tlv320aic32x4 | grep  Failed`
	if [ "$Kernel1V3OnBoard2V0" = "" ];
	then
		if [ "$Kernel2V0OnBoard1V3" = "" ];
		then
			if [ ! "$Kernel1V3" = "" ];
			then
				hwBoardType=BOARD_V1.3
			else
				hwBoardType=BOARD_V2.0
			fi
		else
			hwBoardType=BOARD_V1.3
		fi
	else
		hwBoardType=BOARD_V2.0
	fi

	echo "hw board is " ${hwBoardType}
}

updateAutoInstall()
{
	if [ !  -d "/AutoInstall" ];then
		mkdir /AutoInstall
	fi

	oldfilemd5=`md5sum /AutoInstall/AutoInstall.bin | awk '{print $1}'`
	newfilemd5=`md5sum ${targetDir}/AutoInstall.bin | awk '{print $1}'`

	echo ${oldfilemd5}
	echo ${newfilemd5}

	if [[ ! ${oldfilemd5} = ${newfilemd5} ]]; then

		kill -9 `ps |grep AutoInstall.bin | awk '{print $1}'`

		cp ${targetDir}/start.sh /AutoInstall/start.sh
		cp ${targetDir}/AutoInstall.bin /AutoInstall/AutoInstall.bin
		cp ${targetDir}/libACE.so       /AutoInstall/libACE.so
		cp ${targetDir}/libdevmgr.so   /AutoInstall/libdevmgr.so
		cp ${targetDir}/libservernet.so        /AutoInstall/libservernet.so
		cp ${targetDir}/libACE-5.6.1.so        /AutoInstall/libACE-5.6.1.so
		cp ${targetDir}/libcommonlib.so        /AutoInstall/libcommonlib.so
		cp ${targetDir}/libscreenminalplug.so  /AutoInstall/libscreenminalplug.so
		cp ${targetDir}/libsqlite3.so  /AutoInstall/libsqlite3.so
	fi
}


disk_init(){
	disk_size=`fdisk -l | grep sda | grep GB | awk '{print $3}' | cut -d \. -f 1`
	disk=`ls /dev/sda`
	if [ -z ${disk} ]; then
		echo "ERROR !!! NOT detected hard disk, please check the hard disk condition"
		exit 1;
	fi

	if [ ${disk_size} -ge 2000 ]; then      #>=2T
		echo "hard disk size : ${disk_size} GB"
		echo "partition table : GPT"
		cp -rf ${srcDir}/8003/bin/parted/parted /usr/sbin
		cp -rf ${srcDir}/8003/bin/parted/libparted.so.2 /lib
		chmod +x /usr/sbin/parted
		#exit 0	
		echo y | parted /dev/sda mklabel gpt
		parted /dev/sda mkpart primary 0 100%
		echo i | mkfs.ext4 -F /dev/sda1
		mkdir /zigsun
		mount /dev/sda1 /zigsun        

	elif [ ${disk_size} -lt 2000 ]; then    #<2T        
		echo "hard disk size : ${disk_size} GB"
		echo "partition table : MBR"
	else
		echo "ERROR !!! Unknown error"
		exit 1;
	fi

	diskMount=`df | grep "zigsun" | grep "dev"`
	echo $diskMount
	if [ "$diskMount" = "" ];then
		echo "/zigsun isn't mount"
		umount /zigsun
		mkdir /zigsun
		mount /dev/sda /zigsun/
		mount /dev/sda1 /zigsun/
	fi
	diskMount=`df | grep "zigsun" | grep "dev"`
	echo $diskMount
	if [ "$diskMount" = "" ];then
		echo "/zigsun can't mount, exit"
		echo "If the hard disk size is greater than 2T, the kernel must be updated to support GPT/EXT4, please reboot and try again"
		exit 1
	fi
}

envInit()
{
	getkernelstartaddr
	getHwBoardType
	boardType=$1
	kernellink=$2

	mtd_ch=$(cat /proc/mtd | awk 'NR==4{printf $2}')
	if [ ${boardType} = "slave" ];then
		#update uboot and uImage
		cd ${srcDir}/8003/bin/DVR_RDK4.0/${hwBoardType}/slave/
		chmod +x createSymbol.sh
		./createSymbol.sh
		updateUboot ${srcDir}/8003/bin/DVR_RDK4.0/${hwBoardType}/slave/u-boot.bin

		if [ "${mtd_ch}" = "00600000" ];then
			updateuImage ${srcDir}/8003/bin/DVR_RDK4.0/${hwBoardType}/slave/uImage_ep_1080
		else
			updateuImage ${srcDir}/8003/bin/DVR_RDK4.0/${hwBoardType}/slave/uImage
		fi

		fw_setenv bootargs 'mem=512M ddr_mem=2048M console=ttyO0,115200n8 ubi.mtd=4,2048 root=ubi0:rootfs rootwait=1 rootfstype=ubifs rw ip=192.168.1.8:192.168.3.21:192.168.1.1:255.255.0.0::eth0:off vram=24M vmalloc=502M pcie_mem=32M notifyk.vpssm3_sva=0xBFB00000'	
	else
		#udisk hot plug
		cp -rf ${srcDir}/8003/bin/scripts/mount.sh /etc/udev/scripts/
		chmod +x /etc/udev/scripts/*.sh
		rm -rf /etc/udev/scripts/udev_mount.sh /etc/udev/scripts/udev_umount.sh

		#pcie restart manager
		cp -rf ${srcDir}/8003/bin/scripts/restart.sh /home/root/opt/dvr_rdk/ti816x/scripts/
		chmod +x /home/root/opt/dvr_rdk/ti816x/scripts/restart.sh
		cp -rf ${srcDir}/8003/bin/dm8168_board_bin/sys_start /home/root/opt/dvr_rdk/ti816x/bin/
		chmod +x /home/root/opt/dvr_rdk/ti816x/bin/sys_start

		#update uboot and uImage	
		cd ${srcDir}/8003/bin/DVR_RDK4.0/${hwBoardType}/mast/
		chmod +x createSymbol.sh
		if [ ${kernellink} = "interaction" ];then
			./createSymbol.sh interaction
		elif [ ${kernellink} = "portable" ];then
			./createSymbol.sh portable
		else
			./createSymbol.sh
		fi
		updateUboot ${srcDir}/8003/bin/DVR_RDK4.0/${hwBoardType}/mast/u-boot.bin

		if [ "${hwBoardType}" = "BOARD_V2.0" ];then
			if [ ${boardType} = "portable" ];then
				flash_eraseall /dev/mtd2
				usleep 100000
				nandwrite /dev/mtd2 -p ${srcDir}/8003/bin/DVR_RDK4.0/BOARD_V2.0/mast/zigsun_1080_logo.bmp
				fw_setenv bootcmd 'nand read 0x81000000 0x00280000 0x600000 && logo on 0x81000000 0xA0000000 0x81600000 26 60;nboot 0x81000000 nand0 0x880000;bootm 0x81000000'
				updateuImage ${srcDir}/8003/bin/DVR_RDK4.0/${hwBoardType}/mast/uImage_rc_1080
			else
				if [ ${hwBoard2v0mtd3addr} = "880000" ];then
					fw_setenv bootcmd 'nboot 0x81000000 nand0 0x880000;bootm 0x81000000'
					updateuImage ${srcDir}/8003/bin/DVR_RDK4.0/${hwBoardType}/mast/uImage
				else
					fw_setenv bootcmd 'nboot 0x81000000 nand0 0x580000;bootm 0x81000000'
					updateuImage ${srcDir}/8003/bin/DVR_RDK4.0/${hwBoardType}/mast/uImage 
				fi
			fi
		else
			fw_setenv bootcmd 'nboot 0x81000000 nand0 0x580000;bootm 0x81000000'
			updateuImage ${srcDir}/8003/bin/DVR_RDK4.0/${hwBoardType}/mast/uImage
		fi
	fi

	disk_init

	rm -rf /zigsun/*
	rm -rf /usr/local/Trolltech/QtEmbedded-4.6.2-arm

	cp -rf ${srcDir}/8003/bin/scripts/firewareload_${boardType}.sh /etc/init.d/firewareload.sh
	chmod +x /etc/init.d/firewareload.sh
	cp -rf ${srcDir}/8003/bin/scripts/init.sh /home/root/opt/dvr_rdk/ti816x/
	chmod +x /home/root/opt/dvr_rdk/ti816x/init.sh
	cp -rf ${srcDir}/8003/bin/scripts/load.sh /home/root/opt/dvr_rdk/ti816x/
	chmod +x /home/root/opt/dvr_rdk/ti816x/load.sh
	cp -rf ${srcDir}/8003/bin/scripts/check.sh /home/root/opt/dvr_rdk/ti816x/
	chmod +x /home/root/opt/dvr_rdk/ti816x/check.sh
	cp -rf ${srcDir}/8003/bin/scripts/vk3224.sh /lib/modules/
	chmod +x /lib/modules/vk3224.sh
	cp -rf ${srcDir}/8003/bin/scripts/bootmisc.sh /etc/init.d/
	chmod +x /etc/init.d/bootmisc.sh
	cp -rf ${srcDir}/8003/bin/DVR_RDK4.0/*.ko /lib/modules/
	cp -rf ${srcDir}/8003/bin/DVR_RDK4.0/gdbserver /usr/bin/
	#get fpga version
	cp -rf ${srcDir}/8003/bin/dm8168_board_bin/i2cGetFpgaInformation /home/root/opt/dvr_rdk/ti816x/bin/
	chmod +x /home/root/opt/dvr_rdk/ti816x/bin/i2cGetFpgaInformation
	#support ntfs
	cp -rf ${srcDir}/8003/bin/dm8168_board_bin/NTFS-3G/bin/ntfsfix /bin/
	chmod +x /bin/ntfsfix
	cp -rf ${srcDir}/8003/bin/dm8168_board_bin/NTFS-3G/bin/ntfs-3g /bin/       
	chmod +x /bin/ntfs-3g
	cp -rf ${srcDir}/8003/bin/dm8168_board_bin/NTFS-3G/sbin/mkntfs /sbin/
	chmod +x /sbin/mkntfs
	rm /lib/libntfs-3g.so*
	cp -rf ${srcDir}/8003/bin/dm8168_board_bin/NTFS-3G/lib/libntfs-3g.so.88.0.0.so /lib/libntfs-3g.so.88.0.0
	chmod +x /lib/libntfs-3g.so.88.0.0
	ln -s /lib/libntfs-3g.so.88.0.0 /lib/libntfs-3g.so.88
	ln -s /lib/libntfs-3g.so.88.0.0 /lib/libntfs-3g.so

	cp -rf ${srcDir}/8003/bin/etc/profile /etc/
	cp -rf ${srcDir}/8003/bin/etc/interfaces_${boardType} /etc/network/interfaces
	cp -rf ${srcDir}/8003/bin/etc/interfaces_${boardType} /etc/network/interfaces_bak
	cp -rf ${srcDir}/8003/bin/etc/fstab /etc/
	echo "/dev/sda1            /zigsun               ext4        defaults             0  0" >> /etc/fstab
	cp -rf ${srcDir}/8003/bin/etc/shadow /etc/
	cp -rf ${srcDir}/8003/bin/etc/shadow_bak /etc/
	cp -rf ${srcDir}/8003/bin/DVR_RDK4.0/firmware /home/root/opt/dvr_rdk/ti816x/

	rm /usr/sbin/svr_timer.sh
	rm /usr/sbin/recsvr.sh
	rm /usr/sbin/alsa.sh
	rm /usr/sbin/yun_alsa.sh
	ln -s ${targetDir}/svr_timer.sh /usr/sbin/svr_timer.sh
	ln -s ${targetDir}/recsvr.sh /usr/sbin/recsvr.sh
	ln -s ${targetDir}/alsa.sh /usr/sbin/alsa.sh
	ln -s ${targetDir}/yun_alsa.sh /usr/sbin/yun_alsa.sh
	ln -s ${targetDir}/nginx/sbin/nginx /usr/sbin/nginx

	update-rc.d -f firewareload.sh remove
	update-rc.d firewareload.sh start 99 1 2 3 4 5 .



	dir="/zigsun"
	if [ ! -d $dir  ];then
		mkdir $dir
	fi
	echo 7.75 > /sys/class/rtc/rtc1/device/atrim
	sync
}


creatSymbol()
{
	cd ${targetDir}

	rm libfreetype.so
	rm libfreetype.so.6
	rm libz.so
	rm libACE.so
	rm librtmp.so
	rm libmp3lame.so
	rm libmp3lame.so.0
	rm libcurl.so.4 
	rm libcurl.so
	rm libpng.so
	rm libEMDirector.so libEMDirector.so.1 libEMDirector.so.1.0
	rm nginx/html
	rm libACE.so
	rm libavcodec.so 
	rm libavcodec.so.56 
	rm libavdevice.so 
	rm libavdevice.so.56 
	rm libavfilter.so 
	rm libavfilter.so.5 
	rm libavformat.so 
	rm libavformat.so.56 
	rm libavutil.so
	rm libavutil.so.54 
	rm libcrypto.so 
	rm libdum.so 
	rm libinteractionuiview.so 
	rm libinteractionuiview.so.1 
	rm libinteractionuiview.so.1.0 
	rm libpostproc.so 
	rm libQtScript.so.4
	rm libQtScript.so.4.6 
	rm libresipares.so 
	rm libresip.so 
	rm librutil.so 
	rm libssl.so 
	rm libswresample.so 
	rm libswresample.so.1 
	rm libswscale.so 
	rm libswscale.so.3
	rm nginx.bin
	rm librepro.so
	rm libIntelligentScreen.so libIntelligentScreen.so.1 libIntelligentScreen.so.1.0
	rm libinteractionuiview.so libinteractionuiview.so.1 libinteractionuiview.so.1.0
	rm libts-0.0.so.0 libts.so libts.so.0

	ln -s libIntelligentScreen.so.1.0.0 libIntelligentScreen.so
	ln -s libIntelligentScreen.so.1.0.0 libIntelligentScreen.so.1
	ln -s libIntelligentScreen.so.1.0.0 libIntelligentScreen.so.1.0
	ln -s libts.so.0.10.1 libts-0.0.so.0
	ln -s libts.so.0.10.1 libts.so.0
	ln -s libts.so.0.10.1 libts.so

	ln -s libfreetype.so.6.9.0 libfreetype.so
	ln -s libfreetype.so.6.9.0 libfreetype.so.6
	ln -s libzlib.so libz.so
	ln -s libACE-5.6.1.so libACE.so
	ln -s librtmp.so.0 librtmp.so
	ln -s libmp3lame.so.0.0.0 libmp3lame.so
	ln -s libmp3lame.so.0.0.0 libmp3lame.so.0
	ln -s libcurl.so.4.3.0 libcurl.so.4
	ln -s libcurl.so.4.3.0 libcurl.so
	ln -s libpng16.so.16 libpng.so
	ln -s libEMDirector.so.1.0.0 libEMDirector.so
	ln -s libEMDirector.so.1.0.0 libEMDirector.so.1
	ln -s libEMDirector.so.1.0.0 libEMDirector.so.1.0
	ln -s /zigsun/html/ nginx/html
	ln -s libACE-5.6.1.so libACE.so
	ln -s libavcodec.so.56.35.101 libavcodec.so
	ln -s libavcodec.so.56.35.101 libavcodec.so.56
	ln -s libavdevice.so.56.4.100 libavdevice.so
	ln -s libavdevice.so.56.4.100 libavdevice.so.56
	ln -s libavfilter.so.5.14.100 libavfilter.so
	ln -s libavfilter.so.5.14.100 libavfilter.so.5
	ln -s libavformat.so.56.31.100 libavformat.so
	ln -s libavformat.so.56.31.100 libavformat.so.56
	ln -s libavutil.so.54.23.101 libavutil.so
	ln -s libavutil.so.54.23.101 libavutil.so.54
	ln -s libcrypto.so.1.0.0 libcrypto.so
	ln -s libdum-1.12.so libdum.so 
	ln -s libinteractionuiview.so.1.0.0 libinteractionuiview.so
	ln -s libinteractionuiview.so.1.0.0 libinteractionuiview.so.1
	ln -s libinteractionuiview.so.1.0.0 libinteractionuiview.so.1.0
	ln -s libpostproc.so.53 libpostproc.so
	ln -s libQtScript.so.4.6.2 libQtScript.so.4
	ln -s libQtScript.so.4.6.2 libQtScript.so.4.6
	ln -s libresipares-1.12.so libresipares.so
	ln -s libresip-1.12.so libresip.so
	ln -s librutil-1.12.so librutil.so
	ln -s libssl.so.1.0.0 libssl.so
	ln -s libswresample.so.1.1.100 libswresample.so
	ln -s libswresample.so.1.1.100 libswresample.so.1
	ln -s libswscale.so.3.1.101 libswscale.so
	ln -s libswscale.so.3.1.101 libswscale.so.3
	ln -s nginx/sbin/nginx nginx.bin
	ln -s librepro-1.12.so librepro.so


	if [ -d "${targetDir/ffmpeg_lib/}" ];then
		cd ${targetDir}/ffmpeg_lib/
	else
		cd ${targetDir}
	fi
	./ffmpeglib_symbol.sh

	sync
}

updateUboot()
{
	name=$1

	if [ -f "$name" ];then
		flash_eraseall /dev/mtd0
		usleep 100000
		nandwrite /dev/mtd0 -p  ${name}
	else
		echo "$name non-existent."
	fi

}

updateuImage()
{
	name=$1
	if [ -f "$name" ];then
		flash_eraseall /dev/mtd3
		usleep 100000
		nandwrite /dev/mtd3 -p  ${name}
	else
		echo "$name non-existent."
	fi

}

updateDb()                                                                       
{                                                                                
	OLD_DB="/zigsun/db/serviceMgr.db"
	NEW_DB="/zigsun/html/update.sql"                                      
	if [ -z "${NEW_DB}" ]|| [ -z "${OLD_DB}" ];then                                  
		echo "no db file to update"                                              
	else                                                                             
		echo "updatedb.bin "${OLD_DB} ${NEW_DB}                                  
		cd ${targetDir} 
		./updatedb.bin ${OLD_DB} ${NEW_DB}
	fi                                                             
}  


installQt()
{
	QtDir="/usr/local/Trolltech"
	if [ ! -d ${QtDir}  ];then
		mkdir -p ${QtDir}
	fi

	tar zxvf ${srcDir}/QT/QtEmbedded-4.6.2-arm.tar.gz -C ${QtDir}
	#   tar zxvf ${srcDir}/QT/tslib1.4-arm.tar.gz -C ${QtDir}
	#   cp ${QtDir}/tslib1.4-arm/etc/ts.conf /opt/
	#   cp -rf ${QtDir}/tslib1.4-arm/lib/libts-1.0.so.0* /usr/lib/
	sync
}

#update to disk
copyToDiskUpdate()
{
	diskMount=`df | grep "zigsun" | grep "dev"`
	echo $diskMount
	if [ "$diskMount" = "" ];then
		echo "/zigsun(disk) isn't found"
		umount /media/sda
		umount /media/sda1
		mount /dev/sda /zigsun
		mount /dev/sda1 /zigsun
	fi

	diskMount=`df | grep "zigsun" | grep "dev"`
	if [ "$diskMount" = "" ];then
		echo "/zigsun(disk) isn't found"
		echo "If the hard disk size is greater than 2T, the kernel must be updated to support GPT/EXT4, please reboot and try again"
		exit 1
	fi

	dir="/zigsun/bin/linux"
	if [ ! -d $dir ];then
		mkdir -p $dir	 
	fi

	cp -rf ${srcDir}/8003/bin/DVR_RDK4.0/firmware /home/root/opt/dvr_rdk/ti816x/
	cp -rf ${srcDir}/bin/version_bin.xml /zigsun/bin/
	#cp -rf ${srcDir}/db /zigsun/
	#cp -rf ${srcDir}/bin/linux/etc /zigsun/bin/linux/
	cp -rf ${srcDir}/bin/linux/bin.debug.Linux.Arm/ /zigsun/bin/linux/
	cp -rf ${srcDir}/html /zigsun/
	chmod +x /zigsun/bin/linux/bin.debug.Linux.Arm -R
}

#copy to disk
copyToDisk()
{
	copyToDiskUpdate
	if [ ! -d "/zigsun/db/" ];then
		mkdir -p /zigsun/db
		cp /zigsun/html/serviceMgr.db /zigsun/db
	else
		cp -rf /zigsun/db/serviceMgr.db /zigsun/db/serviceMgr_updatebak.db
		#cp -rf ${srcDir}/db /zigsun/
		updateDb                                                             
	fi
	cp -rf ${srcDir}/bin/linux/etc /zigsun/bin/linux/
}

#copy to disk
copyToDiskUpdateWithEtc()
{
	copyToDiskUpdate
	cp -rf ${srcDir}/bin/linux/etc /zigsun/bin/linux/
}

copyToMastFlash()
{
	dir="/zigsun"
	if [ ! -d $dir  ];then
		mkdir $dir
	fi
	rm -rf /zigsun/bin
	cp -rf ${srcDir}/bin_mast /zigsun/bin
	cp -rf ${srcDir}/8003/bin/DVR_RDK4.0/firmware /home/root/opt/dvr_rdk/ti816x/
	chmod +x /zigsun/bin/linux/bin.debug.Linux.Arm -R
}

copyToSlaveFlash()
{
	dir="/zigsun"
	if [ ! -d $dir  ];then
		mkdir $dir
	fi
	#rm -rf /zigsun/*
	if [ -f "/zigsun/db/serviceMgr.db" ];
	then
		cp /zigsun/db/serviceMgr.db /zigsun/db/serviceMgr_updatebak.db
		cp /zigsun/bin/linux/etc/transcodeCfg.ini /zigsun/bin/linux/etc/transcodeCfg_updatebak.ini
	fi
	tar zxvf ${srcDir}/slavesysRecBin.gz -C /
	if [ -f "/zigsun/db/serviceMgr_updatebak.db" ];
	then
		mv /zigsun/db/serviceMgr_updatebak.db /zigsun/db/serviceMgr.db
		mv /zigsun/bin/linux/etc/transcodeCfg_updatebak.ini /zigsun/bin/linux/etc/transcodeCfg.ini
	fi
	chmod +x /zigsun/bin/linux/bin.debug.Linux.Arm -R
}


createSd()
{
	dir=${sdSrcDir}
	if [ ! -d $dir  ];then
		echo $dir" isn't found"
		return
	fi

	dir=${sdTargetDir}
	if [ ! -d $dir  ];then
		echo $dir" isn't found"
		return
	fi

	dir=${sdTargetDir}/zigsun/bin/linux/bin.debug.Linux.Arm
	if [ ! -d $dir  ];then
		mkdir -p $dir
	fi

	otherDir=${sdTargetDir}/zigsun/8003/
	if [ ! -d $otherDir  ];then
		mkdir -p $otherDir
	fi

	mastDir=${sdTargetDir}/zigsun/bin_mast/linux/bin.debug.Linux.Arm
	if [ ! -d $mastDir  ];then
		mkdir -p $mastDir
	fi

	slaveDir=${sdTargetDir}/zigsun/bin_slave/linux/bin.debug.Linux.Arm
	if [ ! -d $slaveDir  ];then
		mkdir -p $slaveDir
	fi


	cp -rf ${sdSrcDir}/db ${sdTargetDir}/zigsun/
	cp -rf ${sdSrcDir}/html ${sdTargetDir}/zigsun/
	cp -rf ${sdSrcDir}/bin/linux/etc ${sdTargetDir}/zigsun/bin/linux/
	cp -rf ${sdSrcDir}/bin/linux/bin.debug.Linux.Arm/ ${sdTargetDir}/zigsun/bin/linux/

	cd ${sdSrcDir}/bin/linux/bin.debug.Linux.Arm/

	cp avlinkMgrTest.bin libfreetype* libcommonlib.so libsqlite3.so libmp3lame.so* ffserver *.sh ${mastDir}
	cp -rf ../etc ${mastDir}/../
	cp avlinkMgrTest.bin libfreetype* libcommonlib.so libsqlite3.so libmp3lame.so* ffserver *.sh ${slaveDir}
	cp -rf ../etc ${slaveDir}/../

	cp -rf ${sdSrcDir}/8003/bin ${otherDir}
	cp -rf ${sdSrcDir}/8003/dm8168_test ${otherDir}

	cd ${sdTargetDir}
	find . -type d -name ".svn"|xargs rm -rf
}

showVersion()
{
	set +x
	echo "*************************** Version ***********************************"
	echo "Recsys version: " `cat /zigsun/bin/version_bin.xml`
	if [ -f "/zigsun/html/version_html.xml" ];then
		echo "Html version: " `cat /zigsun/html/version_html.xml`
	fi
	echo "Firmware version: " `cat /home/root/opt/dvr_rdk/ti816x/firmware/version_firmware.xml`
	echo "Kernel version: " `uname -a`
	if [ -f "/tmp/fpgaInformation.log" ];then
		echo `cat /tmp/fpgaInformation.log`
	fi
	echo "***********************************************************************"
}

if [ $cmd = "init" ];then
	envInit mast
	installQt
elif [ $cmd = "initSlave" ];then
	envInit slave
elif [ $cmd = "initCloud" ];then
	envInit cloud 
	installQt
elif [ $cmd = "initInteractCloud" ];then
	envInit cloud interaction 
	installQt
elif [ $cmd = "initWallhang" ];then
	envInit cloud interaction
	installQt
elif [ $cmd = "initPortable" ];then
	envInit portable portable
	installQt
elif [ $cmd = "disk" ];then
	copyToDisk
	creatSymbol
	showVersion
elif [ $cmd = "diskupdate" ];then
	copyToDiskUpdate
	creatSymbol
	showVersion
elif [ $cmd = "diskupdateWithEtc" ];then
	copyToDiskUpdateWithEtc
	creatSymbol
	showVersion
elif [ $cmd = "mast" ];then
	copyToMastFlash	
	updateAutoInstall
	creatSymbol
	showVersion
elif [ $cmd = "slave" ];then
	copyToSlaveFlash	
	updateAutoInstall
	creatSymbol
	showVersion
elif [ $cmd = "create" ];then
	createSd
else
	echo "Usage: ./envinit.sh <init|initSlave|initCloud|initInteractCloud|initWallhang|initPortable|disk|diskupdate|diskupdateWithEtc|mast|slave|create|help>"	
fi

