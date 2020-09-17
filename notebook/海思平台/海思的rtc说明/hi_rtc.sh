#!/bin/sh

usage(){
	echo "just synchronizing system time : ./hi_rtc.sh"
	echo "set system time : ./hi_rtc.sh 2012/7/15/13/37/59"
}

get_time_form_hi_rtc(){
	year=`hi_time -g time 	| grep year 	| awk '{print $2}'`
	month=`hi_time -g time 	| grep month 	| awk '{print $2}'`
	date=`hi_time -g time 	| grep date 	| awk '{print $2}'`
	hour=`hi_time -g time 	| grep hour 	| awk '{print $2}'`
	minute=`hi_time -g time | grep minute 	| awk '{print $2}'`
	second=`hi_time -g time | grep second 	| awk '{print $2}'`
	echo "got time : ${year}.$month.$date-$hour:$minute:$second"
}

set_time_to_date(){
	if [ -n "$1" ];then
		hi_time -s time $1
	fi
	get_time_form_hi_rtc
	if [ -z "$year" ] || [ -z "$month" ] || [ -z "$date" ] \
		|| [ -z "$hour" ] || [ -z "$year" ] || [ -z "$year" ];then
		echo "get time error, exit"
		exit 1
	fi
	time="${year}-${month}-${date} ${hour}:${minute}:${second}"
	date -s "$time"
}

wait_sys_ready() {
	for i in $(seq 1 $1)
	do
		rtc_dev=`ls /dev/hi_rtc`
		if [ -z "$rtc_dev" ];then
			echo "/dev/hi_rtc does not exist"
			sleep 1
		fi
	done

	rtc_dev=`ls /dev/hi_rtc`
	if [ -z "$rtc_dev" ];then
		echo "/dev/hi_rtc does not exist, time set failed"
		exit 1
	fi
}


main(){
	wait_sys_ready 5
	set_time_to_date $1
	hi_time -b ON
	exit 0
}

main $1