# srdnlenctf 2022

## Fancy e (11 solves)

The service on this challenge offers you the possibility to generate some RSA parameters in a peculiar way. Indeed, the public exponent e is generated using `p`, `q`, a secret number referred as `base`, and some random numbers and operators. 

For each RSA parameters generated during the same session, it also gives you the same flag encrypted with those parameters. Can you recover it?

### Solution

Disclaimer: There is an uninteded solution to this challenge, but I noticed it when the CTF had already started...

Indeed, when the random numbers assume a specific configuration, the resulting public exponent e is equivalent to... ONE.
And so, the flag is in the ciphertext.

Anyways, here is also how I intended the challenge to be solve.

The crucial part of the challenge is the generation of the public exponent e. If you write down the equations and look carefully at it, you will see that when the random parameters and operators assume a specific value, then e will be equal to `base*phi*k+1` or `base*phi*k+2`.

This will happen when:
```
op2 = -1 (prob. 1/2)
rand2 = 1(prob. 1/5)
op3 = -1 (prob. 1/2)
rand3 = 1 (prob. 1/5)
rand4 = 1 (prob. 1/5)
```
Thus, it will statistically happen once every 500 generated parameters.

While tou could bruteforce the value of `k` during decryption, you cannot do the same for the base, since it is bigger than 300 and it will take much time.

However, the base is ALMOST a common factor of all the generated public exponents, except for a random added value of 1 or 2 at the end, and the value assumed by `op1` and `rand1`.

Thus, it can be statistically recovered as well.

Once you gather the value of the base, you can bruteforce `k` while trying to decrypt all the ciphertexts until you get the flag.

In the solution, we take the file `example.txt` as reference.

### Exploit 1 - Find the base

(This is a step-by-step solution)

```python
#!/bin/env python3

import statistics
from math import gcd
from collections import Counter

vals = open("example.txt", "r").read().split("\n\n")
list_of_e = [int(v.split("\n")[1].split("=")[1]) for v in vals]

# base is a common factor of either e-1 or e-2 for multiple exponents
# We know that base is greater than 300.
lg = []
for e in list_of_e:
    current_e = [e - 1, e - 2]
    for i in list_of_e:
        if i != e:
            current_i = [i - 1, i - 2]
            for j in current_e:
                for k in current_i:
                    g = gcd(j, k)
                    if g != 1 and g > 300:
                        lg.append(g)

# now we have a list of all the common factors.
# since we also have some small factors k and f in the results,
# we eliminate the small factors from the numbers we got (i.e from 1 to 15)
factors = []
for g in lg:
    for i in range(1, 15):
        if g % i == 0 and 300 < g // i:
            factors.append(g // i)

# Then we count each occurrence and frequencies and define a threshold on the obtained data
occ = Counter(factors)
freq = []
threshold = 2000
for k in occ:
    if occ[k] > threshold:
        freq.append(k)

# Since op1 is between -5 and 5 (0 excluded),take the values with the lowest variance and do the average
poss = []
for i in freq:
    for j in freq:
        if i != j:
            if abs(i - j) <= 10:
                poss.append(i)
poss = Counter(poss)
print(f"RESULTS: {poss}")

base_vals = []
for k in poss:
    if poss[k] == max(poss.values()):
        base_vals.append(k)

print(base_vals)
print(f"BASE: {statistics.mean(base_vals)}")

```

### Exploit 2 - Find the flag

(This is a step-by-step solution)

```python
#!/bin/env python3


from Crypto.Util.number import long_to_bytes

vals = open("example.txt", "r").read().split("\n\n")
splt_vals = [v.split("\n") for v in vals]
ct_list = []
e_list = []
n_list = []
for v in splt_vals:
    ct_list.append(int(v[0].split("=")[1]))
    e_list.append(int(v[1].split("=")[1]))
    n_list.append(int(v[2].split("=")[1]))


for e, ct, n in zip(e_list, ct_list, n_list):
    e1 = e-1
    e2 = e-2
    b = 7777

    rand1 = [1, 2, 3, 4, 5, -1, -2, -3, -4, -5]
    poss_r = []
    poss_phik = []
    for r in rand1:
        new_b = b + r
        if e1 % new_b == 0:
            poss_phik.append(e1 // new_b)
            poss_r.append(r)

        if e2 % new_b == 0:
            poss_phik.append(e2 // new_b)
            poss_r.append(r)

        for i in range(len(poss_r)):
            k = abs(poss_r[i]) + 2
            phi = poss_phik[i] // k
            try:
                d = pow(e, -1, phi)
                pt = pow(ct, d, n)
                pt = long_to_bytes(pt)
                if b"srdnlen" in pt:
                    print(pt)
            except ValueError:
                pass
```
