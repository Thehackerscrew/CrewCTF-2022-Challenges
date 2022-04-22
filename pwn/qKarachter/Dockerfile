FROM ubuntu:20.04

RUN apt-get -y update && \
    apt-get -y install xinetd && \
    apt-get -y install qemu-system

RUN useradd -m ctf

ADD start.sh /root
ADD rootfs.cpio /home/ctf
ADD bzImage /home/ctf
ADD service.sh /home/ctf
ADD run.sh /home/ctf
ADD chall.xinetd /etc/xinetd.d/chall

RUN chmod +x /root/start.sh && \
    chmod +x /home/ctf/service.sh && \
    chmod +x /home/ctf/run.sh && \
    chown -R root:ctf /home/ctf && \
    chmod -R 750 /home/ctf/   && \
    chmod 755 /tmp

EXPOSE 1337
ENTRYPOINT  /root/start.sh
