#!/bin/sh

nginx_path=$1
mem_path=$(dirname /data/measure-mem.sh)
cpu_path=$(dirname /data/measure-cpu.sh)
disk_path=$(dirname /data/measure-disk-random.sh)
fork_path=$(dirname /data/measure-fork.sh)

flush_cache() {
	rm -rf /etc/nginx/cache/*
}

for i in `seq 1 48`; do
    flush_cache

	echo $(printf "%s\n" $(date +%s)",";./$cpu_path/measure-cpu.sh) >> /$cpu_path/measure-cpu.csv
	flush_cache

	echo $(printf "%s\n" $(date +%s)",";./$mem_path/measure-mem.sh) >> /$mem_path/measure-mem.csv
	flush_cache

	echo $(printf "%s\n" $(date +%s)",";./$disk_path/measure-disk-random.sh) >> /$disk_path/measure-disk-random.csv
	flush_cache

	echo $(printf "%s\n" $(date +%s)",";./$fork_path/measure-fork.sh) >> /$fork_path/measure-fork.csv
	flush_cache	

	echo $(printf "%s\n" $(date +%s)",";./measure-nginx.sh $nginx_path) >> measure-nginx.csv
done