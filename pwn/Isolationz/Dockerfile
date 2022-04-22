FROM ubuntu:20.04

RUN apt-get -y update && \
    apt-get -y install xinetd

RUN useradd -m ctf

ADD start.sh /root
ADD libc.so.6 /home/ctf
ADD ld-linux-x86-64.so.2 /home/ctf
ADD chall /home/ctf
ADD flag /home/ctf
ADD run.sh /home/ctf
ADD chall.xinetd /etc/xinetd.d/chall

RUN chmod +x /root/start.sh && \
   chmod +x /home/ctf/chall && \
   chmod +x /home/ctf/ld-linux-x86-64.so.2 && \
   chmod +x /home/ctf/libc.so.6 && \
   chmod +x /home/ctf/run.sh && \
   chown -R root:ctf /home/ctf && \
   chmod -R 750 /home/ctf/ && \
   chown root:ctf /home/ctf/flag && \
   chmod 440 /home/ctf/flag 

EXPOSE 1337
ENTRYPOINT  /root/start.sh
