# srdnlenctf 2022

## Breach Paradigm (8 solves)

The challenge is a single binary file (x64, Linux).
The binary offers an interactive window handled by ncurses
where the user can select a sequence of bytes from the ones
offered on screen. If the sequence is correct, the program prints the flag.

### Solution

- Reverse the main program logic to find the validation code.
- It's a custom VM with 5 different operations. Operates on a few static length arrays, not stack-based.
- Analyze the VM to understand the individual opcodes. You should probably make a disassembler.
- Analyze the VM program to understand the instruction flow.
- Transform the instruction flow to track which parts of the code are relevant to the final check.
- The problem can be translated into a linear system. Send it to Z3.
- Input the solution or decrypt it yourself.

### Exploit

```python

off_start = 0xE41C0
off_end = 0xED6CE
with open('./breach', 'rb') as binfile:
    binfile.seek(off_start)
    pr = binfile.read(off_end - off_start)


rev_pr = pr[::-1]

# ops:
OP_ADDR = 0x65
OP_MULL = 0x52
OP_SUBQ = 0x22
OP_CHK = 0x77
OP_LOADF = 0x33

# CHK tests cells Q 0..15
# We want to follow SUBQ operations (only way to interact with cell group)

# Let's take advantage of the RISC setup
ops = list(enumerate(pr[::3]))
subs = [3*x[0] for x in ops if x[1] == 0x22]
print(subs)
# Write down the registers subtracted to the 16 Q operations (parameter 2)
regs = [pr[o+1] for o in subs]
print(regs)

pairs = list(zip(subs, regs))

print(pairs)

# Follow taint of marked registers until we find a LOADF (resets register state with literal from user input)
# We are interested in ops that write to registers.
# LOADF: param 2, stop when found
# MULL: param 1
# ADDR: param 1
# SUBQ, CHK: not interested

# Test with element 1

offs = []

target_regs = set([pairs[0][1]])
for pot_op in range(pairs[0][0], -1, -3):
    if pr[pot_op] == OP_ADDR and pr[pot_op+1] in target_regs:
        print("{}: ADDR {} {}".format(pot_op, pr[pot_op+1],pr[pot_op+2]))
        target_regs.add(pr[pot_op+2]) # Follow taint
        offs.append(pot_op)
        #print(f"ADDED {pr[pot_op+2]}")
    elif pr[pot_op] == OP_MULL and pr[pot_op+1] in target_regs:
        print("{}: MULL {} {}".format(pot_op, pr[pot_op+1],pr[pot_op+2]))
        offs.append(pot_op)
    elif pr[pot_op] == OP_LOADF and pr[pot_op+2] in target_regs:
        print("{}: LOADF {} {}".format(pot_op, pr[pot_op+1],pr[pot_op+2]))
        target_regs.remove(pr[pot_op+2])
        offs.append(pot_op)
        #print(f"REMOVED {pr[pot_op+2]}")

# This works.
# It seems there is a pattern of the type loadf -> mull -> addr (x16) -> subq
# Write the linear equation for row 1
# Pattern: LOADF MULL ADDR

offs= offs[::-1]
op_pattern = [offs[x:x+3] for x in range(0, len(offs),3)]
for o in op_pattern:
    print('flag[{}] * L[{}] + '.format(pr[o[0]+1], pr[o[1]+2]), end='')
print(f"(-Q[{pr[pairs[0][0]+2]}]) == 0")


# But it doesn't have to be in order. It's best to check it manually.

def taint_reg(off, reg_num, indent=0):
    target_reg = reg_num
    for pot_op in range(off, -1, -3):
        if pr[pot_op] == OP_ADDR and pr[pot_op+1] == target_reg:
            print("{} {}: ADDR {} {}".format('  '*indent, pot_op, pr[pot_op+1],pr[pot_op+2]))
            taint_reg(pot_op, pr[pot_op+2], indent+1)
            #print(f"ADDED {pr[pot_op+2]}")
        elif pr[pot_op] == OP_MULL and pr[pot_op+1] == target_reg:
            print("{} {}: MULL {} {}".format('  '*indent, pot_op, pr[pot_op+1],pr[pot_op+2]))
        elif pr[pot_op] == OP_LOADF and pr[pot_op+2] == target_reg:
            print("{} {}: LOADF {} {}".format('  '*indent, pot_op, pr[pot_op+1],pr[pot_op+2]))
            break

# Test on all rows.

for p in pairs:
    print("#")
    taint_reg(p[0], p[1])
print("########")

# We have confirmed the system does follow the previous pattern completely.
# Here is a simple program that writes the linear equations based on the first row tests.

# We can do all the calculations for L and Q


L = [57,  84,  65,  54,  40,   1, 115,  79,  65,   6, 120, 106, 107,
         56, 110,  51,
       63,  62,  88,  52, 105,  55,  85,  59,  69,  36, 110,  74, 104,
          6,   8,  24,
       91, 117, 102,  63,  27,  41,  24,  65,  28,  40,  98, 101,  15,
         35,  30,  33,
       54,  70,   6,  63,  26,  68,  15,  10,  44,   2,  94, 114,  18,
          9,  43,   2,
       106,  68, 116,  61,  88,  25, 111,  62, 115,  68,  64,  31, 105,
         20, 106,  60,
       105,  86,  82,  57, 102,  90,   0,  29,  55,  58,  51,  85,  52,
         89,  92,  20,
       106,  62,  82,  98,  70,  18, 103,  46, 117, 105,   8,   9,  70,
          6,  53, 127,
       26,  43, 105,  99,  54,  62,  98,  62,  68,  70,  61,   4, 126,
         86,  89,  77,
       42, 114, 123, 117, 124,  82,  65,   9,  78, 109,  34,  55, 116,
        110, 105,  11,
       108,  58,   5,   2, 119, 122,  38, 101,  67, 117,  40,  12, 112,
         27,  77,  71,
       66,   5,  97,   0,  84,   1,  52,  43,  22,  48,  23,  80,  85,
         21,  52, 120,
       58, 108,  40,  24,  16,  61, 124, 111,  53,  49,  15,  57,  78,
         97,  79,  34,
       104, 113,  30,  64, 115, 119,  35,  31,  59,  54,  18,  77,  27,
         31,  25, 118,
       85,  86,  76,  98,  88,   8,   1,  73,  12,  30, 126,   4,  96,
         93, 104,  66,
       113,   5,  53,  22,  32,  87, 104,   8,  83,  97,  10,  19,  34,
         59,  31,  18,
       90,  25,  82,  94, 105,  13, 104,   1,  59, 109,  90,  15, 116,
         97,  42,  19]
Q = [67616, 67942, 52969, 28940, 82637, 68203, 76064, 81994, 96139,
       75107, 57013, 67631, 66997, 68933, 49227, 74124]

A = []
B = []

for p in pairs:
    offs = []
    row = [0] * 16

    target_regs = set([p[1]])
    for pot_op in range(p[0], -1, -3):
        if pr[pot_op] == OP_ADDR and pr[pot_op+1] in target_regs:
            #print("{}: ADDR {} {}".format(pot_op, pr[pot_op+1],pr[pot_op+2]))
            target_regs.add(pr[pot_op+2]) # Follow taint
            offs.append(pot_op)
            #print(f"ADDED {pr[pot_op+2]}")
        elif pr[pot_op] == OP_MULL and pr[pot_op+1] in target_regs:
            #print("{}: MULL {} {}".format(pot_op, pr[pot_op+1],pr[pot_op+2]))
            offs.append(pot_op)
        elif pr[pot_op] == OP_LOADF and pr[pot_op+2] in target_regs:
            #print("{}: LOADF {} {}".format(pot_op, pr[pot_op+1],pr[pot_op+2]))
            target_regs.remove(pr[pot_op+2])
            offs.append(pot_op)
            #print(f"REMOVED {pr[pot_op+2]}")
            
    offs= offs[::-1]
    op_pattern = [offs[x:x+3] for x in range(0, len(offs),3)]
    for o in op_pattern:
        print('flag[{}] * {} + '.format(pr[o[0]+1], L[pr[o[1]+2]]), end='')
        row[pr[o[0]+1]] =  L[pr[o[1]+2]]
    print(f"(-{Q[pr[p[0]+2]]}) == 0")
    A.append(row)
    B.append(Q[pr[p[0]+2]])
print(A)
print(B)

# Solve these however you want. Z3 with byte constraints works.

# Being a full rank matrix, it can also be solved directly with numpy. 
# Make sure you're rounding up to the integer and not just casting to int.

import numpy as np 

A_arr = np.array(A)
B_arr = np.array(B)

print([hex(int(x)) for x in np.rint(np.linalg.solve(A_arr, B_arr))])

```
