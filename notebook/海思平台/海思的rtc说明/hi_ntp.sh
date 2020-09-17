ntp_ip=120.25.108.11
#you can get ip from http://www.ntp.org.cn/pool.php
#ping that ip to check stability
check_time(){
	year=`hi_time -g time 	| grep year 	| awk '{print $2}'`
	month=`hi_time -g time 	| grep month 	| awk '{print $2}'`
	date=`hi_time -g time 	| grep date 	| awk '{print $2}'`
	hour=`hi_time -g time 	| grep hour 	| awk '{print $2}'`
	minute=`hi_time -g time | grep minute 	| awk '{print $2}'`
	second=`hi_time -g time | grep second 	| awk '{print $2}'`

	rtc_time="${year}-${month}-${date} ${hour}:${minute}:${second}"
	rtc_time=`date -d "$rtc_time" +%s`
	echo "rtc_time = $rtc_time"
	date_time=`date +%s`
	echo "date_time = $date_time"

	if [ "$rtc_time" != "$date_time" ];then
		date_time=`date "+%Y/%m/%d/%H/%M/%S"`
		hi_time -s time "$date_time"
		echo "synchronize network time successfully, time is $date_time"
	fi
}
get_time_form_ntp(){
	while [ 1 ]
	do
		ntp_state=`ntpclient -s -d -c 1 -i 5 -h $ntp_ip | grep "set time"`
		#echo $ntp_state
	    if [ -n "$ntp_state" ];then
	    	check_time
		fi
		sleep 3
	done
}
main(){
	get_time_form_ntp
	exit 0
}
main