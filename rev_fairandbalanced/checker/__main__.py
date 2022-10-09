#!/bin/env python3

from pwn import *
import logging
import os
logging.disable()

HOST = os.environ.get("HOST", "fab.challs.srdnlen.it")
PORT = int(os.environ.get("PORT", 15000))

r = remote(HOST, PORT)
r.recvuntil(b"NOW")

# One of the many winning states
r.send(b"ffffffffe1024598sneseswwnensennnswennnnesweennenwwnswwwwnewesessssgg")
r.recvuntil("srdnlen{")
flag = r.recvuntil("}")
r.close()

print(str(b"srdnlen{"+flag, "utf8"))
