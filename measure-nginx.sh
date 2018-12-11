#!/bin/sh

path_to_file=$1
max_time=10

# Create 5 request for the same testfile to run in parallel with a timeout at 10s
for i in `seq 1 2`;
    do
		timeout $max_time wget $path_to_file > /dev/null 2>&1 &
    done    

# Try to access the file within 10 seconds
time_taken=$( { timeout $max_time time wget $path_to_file -args; } 2>&1 | grep "elapsed" | awk '{printf $3}' | sed 's/[^0-9.:]//g')

# Wait for all processes to finish, remove downloaded files and print the time it took to downlad the file with 2 more requests in parallel
wait
rm testfile*
echo "$time_taken"