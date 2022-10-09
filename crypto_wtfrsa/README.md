# srdnlenctf 2022

## Template (34 solves)

Why can't i decrypt this?

### Solution

If there are no proper checks, RSA keys can be generated incorrectly.
In this challenge the public exponent (e) is not coprime with $\phi$(N), the following paper analyzes the problem of recovering plaintexts encrypted to incorrectly generated RSA keys.
https://eprint.iacr.org/2020/1059.pdf

### Exploit

```python
#!/bin/env python3

from Crypto.Util.number import long_to_bytes, bytes_to_long
from tqdm import tqdm

def find_multiplicative_generator(p, q, e):
    phi = (p-1) * (q-1)
    phi = phi // e
    g = 1
    g_E = 1
    while(True):
        g += 1
        g_E = pow(g, phi, p*q)
        if g_E != 1:
            return g_E

# If a part of flag is known (for example ctfname{) insert it to filter messages
def find_plaintext(p, q, e, ct, known_flag=b""):
    g = find_multiplicative_generator(p, q, e)
    n = p*q
    phi = (p-1) * (q-1)
    phi = phi // e
    d = pow(e, -1, phi)
    assert d*e % phi == 1
    a = pow(ct, d, n)
    l = 1 % n
    for i in tqdm(range(1, e)):
        x = (a*l) % n
        if known_flag in long_to_bytes(x).lower():
            print(long_to_bytes(x))
            print()
        l = (l*g) % n

e = 18959
p = 8853107629856302430942645802685600792214004993921099904332911487775152756152460899671437787731654521568200225685173143721860070387195312109191089843558621
q = 12263776399134581413994039043220106353464473125114825391625856240762676598269365363349978019785253746863903410731653514543481130557521535535237879154364911
ct = 64521812048352846958817059534278142356568238192123182336017359260377716295619478728140210232152018155950695896362673540987021049139829121799099909484852120051863107269165139203886417085008081775352265576110683356959797391197297615443422020648048621511483229468510937180464189390129089235915976695524813058244

##### Solution #######
find_plaintext(p, q, e, ct, b"srdnlen")

```
