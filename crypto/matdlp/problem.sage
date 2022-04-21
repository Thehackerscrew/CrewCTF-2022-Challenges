FLAG = open('flag.txt', 'r').read().encode()

p = 0x3981e7c18d9517254d5063b9f503386e44cd0bd9822710b4709c89fc63ce1060626a6f86b1c76c7cbd41371f6bf61dd8216f4bc6bad8b02a6cd4b99fe1e71b5d9ffc761eace4d02d737e5d4bf2c07ff7
m = 6

import random
from hashlib import sha256
from Crypto.Cipher import AES
from Crypto.Util import Padding

K = GF(p)
matspace = MatrixSpace(K, m)

while(True):
    U = matspace.random_element()
    if U.determinant() != 0:
        break

print('U={}'.format(U.list()))

while(True):
    l1, l2, l3 = K.random_element(), K.random_element(), K.random_element()
    X = matrix(K, m, m, [l1,0,0,0,0,0]+[0,l2,1,0,0,0]+[0,0,l2,0,0,0]+[0,0,0,l3,1,0]+[0,0,0,0,l3,1]+[0,0,0,0,0,l3])
    if U*X != X*U:
        break

print('X={}'.format(X.list()))

def genkey():
    t = random.randint(1, p-1)
    s = random.randint(1, p-1)
    Us = U**s
    privkey = (t, s)
    pubkey = Us * (X**t) * Us.inverse()
    return (privkey, pubkey)

alicekey = genkey()
bobkey = genkey()

print('alice_pubkey={}'.format(alicekey[1].list()))
print('bob_pubkey={}'.format(bobkey[1].list()))

alice_Us = (U**alicekey[0][1])
bob_Us = (U**bobkey[0][1])
sharedkey_a = alice_Us * (bobkey[1]**alicekey[0][0]) * alice_Us.inverse()
sharedkey_b = bob_Us * (alicekey[1]**bobkey[0][0]) * bob_Us.inverse()

assert sharedkey_a == sharedkey_b

aeskey = sha256(b''.join([int.to_bytes(int(sharedkey_a[i][j]), length=80, byteorder='big') for i in range(m) for j in range(m)])).digest()

cipher = AES.new(aeskey, AES.MODE_CBC, iv=b'\x00'*16)
ciphertext = cipher.encrypt(Padding.pad(FLAG, 16))
print('ciphertext=0x{}'.format(ciphertext.hex()))

