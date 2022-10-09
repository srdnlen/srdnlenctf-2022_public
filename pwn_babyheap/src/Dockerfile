FROM ubuntu:22.04

RUN apt-get update && apt-get install socat -y && apt-get clean

RUN useradd babyheap 

WORKDIR /home/babyheap
COPY babyheap .
COPY flag.txt .

RUN chown -R root:babyheap .
RUN chmod +x /home/babyheap/babyheap

USER babyheap 

ENTRYPOINT socat -T 30 -d tcp-listen:7337,reuseaddr,fork exec:/home/babyheap/babyheap,stderr
