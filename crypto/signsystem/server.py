import sys
import random
from hashlib import sha256
from Crypto.Util.number import inverse
import ecdsa

from secret import FLAG

curve = ecdsa.curves.SECP112r1
p = int(curve.curve.p())
G = curve.generator
n = int(curve.order)

class SignSystem:
    def __init__(self):
        self.key = ecdsa.SigningKey.generate(curve=curve)
        self.nonce = [random.randint(1, n-1) for _ in range(112)]

    def sign(self, msg):
        e = int.from_bytes(sha256(msg).digest(), 'big') % n
        h = bin(e)[2:].zfill(112)
        k = sum([int(h[i])*self.nonce[i] for i in range(112)]) % n
        r = int((k * G).x()) % n
        s = inverse(k, n) * (e + r * self.key.privkey.secret_multiplier) % n
        return (int(r), int(s))

    def verify(self, msg, sig):
        (r, s) = sig
        e = int.from_bytes(sha256(msg).digest(), 'big')
        if s == 0:
            return False
        w = inverse(s, n)
        u1 = e*w % n
        u2 = r*w % n
        x1 = int((u1*G + u2*self.key.privkey.public_key.point).x()) % n
        return (r % n) == x1

if __name__ == '__main__':
    HDR = 'Welcome to sign system.'
    print(HDR)
    MENU = "1:sign\n2:verify\n3:getflag\n"
    S = SignSystem()
    try:
        while(True):
            print('')
            print(MENU)
            i = int(input('>> '))
            if i == 1:
                msghex = input('msg(hex): ')
                sig = S.sign(bytes.fromhex(msghex))
                print(f'signature: ({hex(sig[0])}, {hex(sig[1])})')
            elif i == 2:
                msghex = input('msg(hex): ')
                sig0hex = input('sig[0](hex): ')
                sig1hex = input('sig[1](hex): ')
                result = S.verify(bytes.fromhex(msghex), (int(sig0hex, 16), int(sig1hex, 16)))
                if result:
                    print('Verification success.')
                else:
                    print('Verification failed.')
            elif i == 3:
                target = random.randint(2**511, 2**512-1)
                print(f'target: {hex(target)}')
                sig0hex = input('sig[0](hex): ')
                sig1hex = input('sig[1](hex): ')
                result = S.verify(bytes.fromhex(hex(target)[2:]), (int(sig0hex, 16), int(sig1hex, 16)))
                if result:
                    print('OK, give you a flag.')
                    print(FLAG.decode())
                    break
                else:
                    print('NG.')
                    break
    except KeyboardInterrupt:
        print('bye')
        sys.exit(0)
    except:
        print('error occured')
        sys.exit(-1)

