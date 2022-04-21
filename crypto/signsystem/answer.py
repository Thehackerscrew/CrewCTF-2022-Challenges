import sys
import random
from hashlib import sha256
from Crypto.Util.number import inverse
import ecdsa

import cypari2

curve = ecdsa.curves.SECP112r1
p = int(curve.curve.p())
G = curve.generator
n = int(curve.order)

from pwn import *

proc = remote('signsystem.crewctf-2022.crewc.tf', 1337)

def sign(msgint):
    proc.recvuntil(b'>> ')
    proc.sendline(b'1')
    proc.recvuntil(b': ')
    msghex = hex(msgint)[2:]
    msghex = '0'*(len(msghex)%2)+msghex
    proc.sendline(msghex.encode())
    proc.recvuntil(b': ')
    (r, s) = tuple(map(lambda x:int(x,16),proc.recvline().strip(b'\n').lstrip(b'(').rstrip(b')').split(b', ')))
    return (r,s)

pari = cypari2.Pari()

mat_h_lst = []
mat_s_e_lst = []
mat_s_r_lst = []
while(True):
    msgint = random.randint(1, 2**256)
    msghex = hex(msgint)[2:]
    msghex = '0'*(len(msghex)%2)+msghex
    msg = bytes.fromhex(msghex)
    (r,s) = sign(msgint)
    e = int.from_bytes(sha256(msg).digest(), 'big') % n
    h = bin(e)[2:].zfill(112)
    mat_h_lst.append(list(map(lambda x:pari.Mod(x,n), h)))
    mat_s_e_lst.append(inverse(s,n) * e % n)
    mat_s_r_lst.append(inverse(s,n) * r % n)

    equ = pari.lindep(mat_h_lst)
    if list(equ) != []:
        break
    print(len(mat_h_lst))

s_e_final = 0
s_r_final = 0
for i,equele in enumerate(equ):
    equele = int(pari.lift(equele))
    s_e_final += equele * mat_s_e_lst[i]
    s_r_final += equele * mat_s_r_lst[i]
privkey = -s_e_final * inverse(s_r_final, n) % n

print(privkey)

print(proc.recvline())
proc.recvuntil(b'>> ')
proc.sendline(b'3')
proc.recvuntil(b': ')
target = bytes.fromhex(proc.recvline().strip(b'\n')[2:].decode())

e = int.from_bytes(sha256(target).digest(), 'big') % n
h = bin(e)[2:].zfill(112)
k = 1
r = int((k * G).x()) % n
s = inverse(k, n) * (e + r * privkey) % n

proc.recvuntil(b': ')
proc.sendline(hex(r)[2:].encode())
proc.recvuntil(b': ')
proc.sendline(hex(s)[2:].encode())

print(proc.recvline())
print(proc.recvline())
