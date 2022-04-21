p = 0x3981e7c18d9517254d5063b9f503386e44cd0bd9822710b4709c89fc63ce1060626a6f86b1c76c7cbd41371f6bf61dd8216f4bc6bad8b02a6cd4b99fe1e71b5d9ffc761eace4d02d737e5d4bf2c07ff7
m = 6

import random
from hashlib import sha256
from Crypto.Cipher import AES
from Crypto.Util import Padding

K = GF(p)
matspace = MatrixSpace(K, m)

exec(open('output.txt','r').read())
U = matrix(K, m, m, U)
X = matrix(K, m, m, X)
alice_pubkey = matrix(K, m, m, alice_pubkey)
bob_pubkey = matrix(K, m, m, bob_pubkey)
ciphertext = int.to_bytes(int(ciphertext), (int(ciphertext).bit_length()+7)//8, 'big')

eigenvalues_alicekeypub = alice_pubkey.eigenvalues()
eigenvalues_X = X.eigenvalues()

powlst = []
for target in list(set(eigenvalues_alicekeypub)):
    for base in list(set(eigenvalues_X)):
        if eigenvalues_alicekeypub.count(target) == eigenvalues_X.count(base):
            alpha = discrete_log(target, base)
            powlst.append((alpha, base.multiplicative_order()))

exp = crt([powlst[i][0] for i in range(len(powlst))], [powlst[i][1] for i in range(len(powlst))])

Xexp = X**exp
# solve Us * Xexp = alicekey[1] * Us, Us*U = U*Us
equlst = []
for i in range(m):
    for j in range(m):
        equlstele = [0]*(m**2)
        for k in range(m):
            equlstele[m*i+k] += Xexp[k][j]
            equlstele[m*k+j] -= alice_pubkey[i][k]
        equlst.append(equlstele)
for i in range(m):
    for j in range(m):
        equlstele = [0]*(m**2)
        for k in range(m):
            equlstele[m*i+k] += U[k][j]
            equlstele[m*k+j] -= U[i][k]
        equlst.append(equlstele)
equ = matrix(K, (m**2)*2, m**2, equlst)
sol = equ.transpose().kernel().basis()
if len(sol) == 0:
    raise ValueError
Ustilde = matrix(K,m,m,sol[0])
if Ustilde.determinant() == 0:
    raise ValueError
pseudo_sharedkey = Ustilde * (bob_pubkey**exp) * Ustilde.inverse()
pseudo_aeskey = sha256(b''.join([int.to_bytes(int(pseudo_sharedkey[i][j]), length=80, byteorder='big') for i in range(m) for j in range(m)])).digest()
print(b'foundkey:'+pseudo_aeskey)
pseudo_cipher = AES.new(pseudo_aeskey, AES.MODE_CBC, iv=b'\x00'*16)
plaintext = pseudo_cipher.decrypt(ciphertext)
print(plaintext)
