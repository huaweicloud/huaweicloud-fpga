#!/bin/sh
#
#-------------------------------------------------------------------------------
#      Copyright 2017 Huawei Technologies Co., Ltd. All Rights Reserved.
# 
#      This program is free software; you can redistribute it and/or modify
#      it under the terms of the Huawei Software License (the "License").
#      A copy of the License is located in the "LICENSE" file accompanying 
#      this file.
# 
#      This program is distributed in the hope that it will be useful,
#      but WITHOUT ANY WARRANTY; without even the implied warranty of
#      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#      Huawei Software License for more details. 
#-------------------------------------------------------------------------------

#######################################################################################################################
## get script path
#######################################################################################################################
if [[ "$0" =~ ^\/.* ]]; then
	script=$0
else
	script=$(pwd)/$0
fi
script=$(readlink -f $script)
script_path=${script%/*}

#######################################################################################################################
## Set the delay time;Initial value is 0
#######################################################################################################################
days=0
hour=0
minutes=0
seconds=0
#######################################################################################################################
## Get the script parameter and running function or change the parameter value
#######################################################################################################################
function usage
{
		#echo "=================================================================================================="
		#echo
		echo "Usage: schedule.sh NUMBER[SUFFIX]  or [HOUR]:[MINUTES]"
		echo "       SUFFIX may be \`s for seconds (the default),\`m for minutes, \`h for hours or \`d for days."
		echo "       ------------+-----------------------------------------------"
		echo "       For example | sh schedule_task.sh 1m      --run shell 1 minute later"
		echo "       For example | sh schedule_task.sh 1h      --run shell 1 hour later"
		echo "       ------------+-----------------------------------------------"
		echo "       For example | sh schedule_task.sh 11:50   --run project at 11:50"
		echo "       For example | sh schedule_task.sh 23:59   --run project at 23:59"	
		echo "       ------------+-----------------------------------------------"

		#echo "================================================================================================="
}
while [ $# -gt 0 ]
do
	case "$1" in
		-h | -H | -help | --help )
			usage
			exit
		;;
		*d )
			days=${1%d}
			;;
		*h )
			hour=${1%h}
			;;
		*m )
			minutes=${1%m}
			;;
		*s )
			seconds=${1%s}
			;;
		*\:* )
			hour=${1%\:*}	
			minutes=${1#*\:}
			if [ $hour -ge 0 -a $hour -lt 24 -a $minutes -ge 0 -a $minutes -lt 60 ]; then
				echo "This project will be run at $hour:$minutes"
				current_hour=$(date +%H)
				hour=$((10#$hour-10#$current_hour))
				if [[ $hour -lt 0 ]]; then
					hour=$((10#$hour+24))
				fi
				current_minute=$(date +%M)
				minutes=$((10#$minutes-10#$current_minute))
				if [[ $hour -eq 0 ]]; then
					if [[ $minutes -lt 0 ]]; then
						hour=$((10#$hour+24))
					fi
				fi
			else
				echo "ERROR: '$1' the time data is wrong"
				echo "        Please input the correct time format: 00:00~23:59"
				exit
			fi
			;;
		* )
			echo "ERROR：'$1' invalid character! "   
			echo "        please input the '-h'，'-H'，'-help' or '--help' character to get help of schedule_task.sh"
			echo
			exit
			;;
		esac
	shift
done
#######################################################################################################################
## source the build.sh
#######################################################################################################################
second=$(($days*24*60*60+$hour*60*60+$minutes*60+$seconds))
source $script_path/build.sh -t $second &
