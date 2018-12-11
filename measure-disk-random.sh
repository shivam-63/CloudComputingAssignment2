#!/bin/bash
BASEDIR=$(dirname "$0")
declare -a results
results=()

# We will run the script for a maximum of 15 seconds (between 10 and 20)
end=$((SECONDS+10))
# We will make sure we don't enter if we have already passed 15
while [ $SECONDS -lt $end ]; do
	# We will use this to timeout our execution
    ((time_left=$end-$SECONDS))

	# The most important command here is timeout
	# We must stop execution after 15 seconds and we will keep running the simulation in a loop
	# Every iteration reduces the available time left
	# This way it is impossible to go over the time limit

	# Run fio random-read, grep the line containing aggregated read speed result
	# get the 3rd item of the output line (aggrb)
	# Leave only digits in the output to get a read speed in KB/s
    result=$(timeout $time_left fio --name=randread --ioengine=libaio --iodepth=2 --rw=randread --bs=4k --direct=0 --size=16M --numjobs=1 --runtime=15 --group_reporting | grep aggrb | awk '{printf $3}' | sed 's/[^0-9.]//g')

	# If we had to timeout our execution than we will only add the result if it is not empty
    if ! [ -z ${result} ] ; then
    	results+=($result)
    fi

done

# This function sorts our array in ascending order and creates a new array from that
IFS=$'\n' sorted_array=($(sort <<<"${results[*]}"))
unset IFS

# We need the length of our array to find the median
length=${#results[@]}

if (( $length % 2 == 0 )) ; then
	# print an average of the 2 middle terms and multiply by 1024 * 1024 to transform MiB/s to B/s
	echo $(echo "scale=0; (${sorted_array[($length-1)/2]} + ${sorted_array[($length-1)/2 + 1]}) / 2 * 1024 * 1024" |bc -l)
else
	# If its odd we just output the middle value
	echo $(echo "scale=0; ${sorted_array[$length / 2]} * 1024 * 1024" | bc -l)
fi
