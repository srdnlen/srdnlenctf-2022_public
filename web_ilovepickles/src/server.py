from pickle import *
from flask import *
import os
import base64
app = Flask(__name__)

FLAG = os.getenv('FLAG')

class User:

    def __init__(self, user_type):
    	self.user_type = user_type


@app.route('/')
def homepage():
   template = """
      <html>
         <body>
            <h1> Welcome to my challenge </h1>
            <a href="/flag">Get flag</a>  
         </body>
      </html>
      """
   user = User("Anonymous")
   resp = make_response(render_template("index.html"))
   resp.set_cookie('userInfo', base64.b64encode(dumps(user)))
   
   return resp
  
@app.route('/flag', methods = ['GET'])
def flag():
	userInfo = request.cookies.get('userInfo')
	user = loads(base64.b64decode(userInfo))
	if (user.user_type == "Anonymous"):
		flag = "You are not an admin!"
	elif(user.user_type == "admin"):
		flag = FLAG
	else:
		flag = "Why are you not Anonymous and not admin? WTF"
	return render_template("admin.html", flag=flag)

if __name__=="__main__":
	app.run("0.0.0.0",port = 5000,debug=False)
