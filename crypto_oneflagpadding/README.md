# srdnlenctf 2022

## One Flag Padding (11 solves)

This challenge proposes a server that takes custom messagges, appends the flag at the end and encrypts it with RSA.
It also provides the encryption of the flag. Can you recover it?

### Solution

Franklin-Reiter attack against RSA.
If two messages differ only by a known fixed difference between the two messages
and are RSA encrypted under the same RSA modulus N
then it is possible to recover both of them.

### Exploit

```python
SageMath required!

from Crypto.Util.number import long_to_bytes, bytes_to_long

# Inputs are modulus, known difference, ciphertext 1, ciphertext2.
# Ciphertext 1 corresponds to smaller of the two plaintexts. (The one without the fixed difference added to it)
def franklinReiter(n,e,r,c1,c2):
    R.<X> = Zmod(n)[]
    f1 = X^e - c1
    f2 = (X + r)^e - c2
    # coefficient 0 = -m, which is what we wanted!
    return Integer(n-(compositeModulusGCD(f1,f2)).coefficients()[0])

  # GCD is not implemented for rings over composite modulus in Sage
  # so we do our own implementation. Its the exact same as standard GCD, but with
  # the polynomials monic representation
def compositeModulusGCD(a, b):
    if(b == 0):
        return a.monic()
    else:
        return compositeModulusGCD(b, a % b)

# We know c1, c2, r, e, n
# c1 = pow(m + 0, e, n)
# c2 = pow(m + r, e, n)

n = 51482698748683767088087774270468156789100783641624785984084944863403828221503510365647069203224514944133755999305691830273348754501244540637720175453826222195986788740756721867756000903482921840078455512554853981304825785510942041958289855014883755566433382271880955982668769167111413833106363196394414775723
e = 7
r = bytes_to_long(b"aaaaaaaaaa")
c1 = 39543566838040231239939164803218710613860645995020757373962281275377085927966049379744197052397001009854135889517120893834339242951683165219271078557757906181200134496843382014432758096445044636698554061314566046396547732647265656862210402239966335508430547937762410030734921297166737744160611698564562209262
c2 = 41946100253688314489554460200533212544861235453293689162120385015587544249894510737558568165826824266673923747032483973391038813083195661342878853206935911092078295829114403226841068488887869839383208080644706526631222059077684047373505456924287431408824857660746171269417541645046015662527642772050914041123

for i in range(150):
    r = bytes_to_long(b"aaaaaaaaaa")*(256**i)
    flag = franklinReiter(n,e,r,c1,c2)
    flag_text = long_to_bytes(flag)
    if b"srdnlen" in flag_text:
        print(flag_text)
        break


```
