FROM ubuntu:18.04

ENV DEBIAN_FRONTEND=noninteractive 

RUN sed -i "s/http:\/\/archive.ubuntu.com/http:\/\/mirrors.tuna.tsinghua.edu.cn/g" /etc/apt/sources.list && \
    apt-get update && apt-get dist-upgrade && \
	apt-get install -y software-properties-common && \
	add-apt-repository ppa:deadsnakes/ppa -y

RUN apt-get update && \
    apt-get install -y xinetd python3.10

RUN useradd -m ctf

WORKDIR /home/ctf

RUN mkdir /home/ctf/dev && \
    mknod /home/ctf/dev/null c 1 3 && \
    mknod /home/ctf/dev/zero c 1 5 && \
    mknod /home/ctf/dev/random c 1 8 && \
    mknod /home/ctf/dev/urandom c 1 9 && \
    chmod 666 /home/ctf/dev/*

RUN mkdir /home/ctf/bin && \
    cp /bin/sh /home/ctf/bin && \
    cp /bin/ls /home/ctf/bin && \
    cp /bin/cat /home/ctf/bin

COPY ./ctf.xinetd /etc/xinetd.d/ctf
COPY ./start.sh /root/start.sh
COPY ./flag.txt /flag.txt

RUN echo "Blocked by ctf_xinetd" > /etc/banner_fail
RUN chmod +x /root/start.sh

COPY ./bin/ /home/ctf/

RUN chown -R root:ctf /home/ctf && \
    chown root:ctf /flag.txt && \
    chmod -R 750 /home/ctf && \
	chmod 750 /flag.txt

EXPOSE 9999
ENTRYPOINT /root/start.sh