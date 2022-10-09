# srdnlenctf 2022

## Easy RSA (24 solves)

The challenge gives us 15 ciphertext using the same public exponent and a different modulus for each ciphertext.
The flag is padded in each ciphertext with a custom padding function, the padding can be driven with the choice of the value k requested in input.

### Solution

The goal of the challenge is to get 15 different ciphertexts of the same plaintext, so have the same padding in each plaintext, in order to use the Chinese reminder theorem to get the value of the plaintext.

The two primes numbers x and y are very small, so the value obtained by multiplying the 2 values can be factored easily.
Having found the two values, which in combination with its multiples can be used as the k value, so that padding is applied only once.

At this point the CRT can be applied to the 15 available ciphertexts with their respective modules, to obtain the 13th power of the flag.
The flag can finally be obtained by calculating the 13th-root of the value obtained.

### Exploit

```python
#!/bin/env python3

from pwn import *
from factordb.factordb import FactorDB
from random import randint
from sympy.ntheory.modular import crt
from gmpy2 import iroot
from Crypto.Util.number import long_to_bytes
import re
import logging

logging.disable()

e = 13
Ns = []
CTs = []

rem = remote("easyrsa.challs.srdnlen.it", 9000)

for _ in range(15):
    rem.recvuntil(b"x*y:")
    n = int(rem.recvline().strip())

    f = FactorDB(n)
    f.connect()
    p, q = f.get_factor_list()

    phi = (p-1) * (q-1)
    k = randint(2**128, 2**130)
    r = phi * randint(2**128, 2**130)

    rem.sendline(str(k).encode())
    rem.sendline(str(r).encode())

    rem.recvuntil(b"N:")
    Ns.append(int(rem.recvline().strip()))
    rem.recvuntil(b"CT:")
    CTs.append(int(rem.recvline().strip()))

    # print(Ns, CTs)

plain_root = crt(Ns, CTs)[0]
plain, isroot = iroot(plain_root, 13)

assert isroot is True

print(f"flag: {re.sub('poba', '', long_to_bytes(plain).decode())}")

```
