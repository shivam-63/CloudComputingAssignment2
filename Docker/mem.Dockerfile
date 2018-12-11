FROM ubuntu:16.04
COPY /src /data

RUN apt-get update
RUN apt-get install -y bc
RUN cd /data

# Please note we used read-save.sh script running all scripts in a loop and writing to files. Benchmarking scripts only print output to the console. In this version of the container, all scripts except for measure-mem.sh were commented. 
ENTRYPOINT ["./data/read-save.sh"]