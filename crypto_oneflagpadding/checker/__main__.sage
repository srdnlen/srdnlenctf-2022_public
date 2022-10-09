#!/usr/bin/env sage

# SageMath required!

import os
from minipwn import *

# Franklin-Reiter attack against RSA.
# If two messages differ only by a known fixed difference between the two messages
# and are RSA encrypted under the same RSA modulus N
# then it is possible to recover both of them.

from Crypto.Util.number import long_to_bytes, bytes_to_long

# Inputs are modulus, known difference, ciphertext 1, ciphertext2.
# Ciphertext 1 corresponds to smaller of the two plaintexts. (The one without the fixed difference added to it)
def franklinReiter(n,e,r,c1,c2):
    R.<X> = Zmod(n)[]
    f1 = X^e - c1
    f2 = (X + r)^e - c2
    # coefficient 0 = -m, which is what we wanted!
    return Integer(n-(compositeModulusGCD(f1,f2)).coefficients()[0])

  # GCD is not implemented for rings over composite modulus in Sage
  # so we do our own implementation. Its the exact same as standard GCD, but with
  # the polynomials monic representation
def compositeModulusGCD(a, b):
    if(b == 0):
        return a.monic()
    else:
        return compositeModulusGCD(b, a % b)

# We know c1, c2, r, e, n
# c1 = pow(m + 0, e, n)
# c2 = pow(m + r, e, n)

# Se challenge tcp
HOST = os.environ.get("HOST", "oneflagpadding.challs.srdnlen.it")
PORT = int(os.environ.get("PORT", 15006))

rem = remote(HOST, PORT)

rem.recvuntil(b"This is the flag: ")
c1 = int(rem.recvline().strip())

rem.sendline(b"aaaaaaaaaa")
r = bytes_to_long(b"aaaaaaaaaa")

rem.recvuntil(b"e: ")
e = int(rem.recvline().strip())

rem.recvuntil(b"n: ")
n = int(rem.recvline().strip())

rem.recvuntil(b"enc_message: ")
c2 = int(rem.recvline().strip())

for i in range(150):
    r = bytes_to_long(b"aaaaaaaaaa")*(256**i)
    flag = franklinReiter(n,e,r,c1,c2)
    flag_text = long_to_bytes(flag)
    if b"srdnlen" in flag_text:
        plain = flag_text
        break

print(plain[plain.index(b"srdnlen"):plain.index(b"}")+1].decode())
