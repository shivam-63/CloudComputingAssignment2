#!/bin/sh

nginx_path=$1

flush_cache() {
	# sync; echo 1 > /proc/sys/vm/drop_caches
	rm -rf /etc/nginx/cache/*
}

for i in `seq 1 48`; do
    flush_cache

	echo $(printf "%s\n" $(date +%s)",";./measure-cpu.sh) >> measure-cpu.csv
	flush_cache

	echo $(printf "%s\n" $(date +%s)",";./measure-mem.sh) >> measure-mem.csv
	flush_cache

	echo $(printf "%s\n" $(date +%s)",";./measure-disk-random.sh) >> measure-disk-random.csv
	flush_cache

	echo $(printf "%s\n" $(date +%s)",";./measure-fork.sh) >> measure-fork.csv
	flush_cache	

	echo $(printf "%s\n" $(date +%s)",";./measure-nginx.sh $nginx_path) >> measure-nginx.csv

done