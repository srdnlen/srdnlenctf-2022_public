#!/bin/env python3

from pwn import *
from factordb.factordb import FactorDB
from random import randint
from sympy.ntheory.modular import crt
from gmpy2 import iroot
from Crypto.Util.number import long_to_bytes
import re
import logging
import os
logging.disable()

e = 13
Ns = []
CTs = []

# Se challenge tcp
HOST = os.environ.get("HOST", "easyrsa.challs.srdnlen.it")
PORT = int(os.environ.get("PORT", 15005))

rem = remote(HOST, PORT)

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

print(f"{re.sub('poba', '', long_to_bytes(plain).decode())}")
