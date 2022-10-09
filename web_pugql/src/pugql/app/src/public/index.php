<?php
include '../config.php';
$conn = mysqli_connect($servername, $username, $password, "pugdatabase");


  function pug_security($string){
    
    # We should improve our security filters! 
    $words_blocklist = [
        "SELECT", "DROP", "FLAG", "OR", "AND", "UNION", "INSERT", "INFORMATION_SCHEMA", "--", "MARIO", "GERRYSCOTTI", "LIKE"
    ];
    if(preg_match('/[#$^&*\-\[\]\`;.\/|":<>?~\\\\]/', $string)){
      return 'bababooey';
    }
    for($i=0; $i < count($words_blocklist); $i++){
        $string = str_replace($words_blocklist[$i], "bababooey", $string);
    }
    return $string;
  }


# Mario's password is tooooooop secret! 

$username = pug_security($_POST['username']);
$passwd = pug_security($_POST['password']);

$result = mysqli_query($conn, "SELECT * FROM users WHERE username='".$username."' AND password='".$passwd."';");


if (mysqli_num_rows($result) == 0) {
echo("<!doctype html>
<html lang=\"en\">
<head>
  <meta charset=\"utf-8\">
  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\">
  <title>PugQLi</title>
  <link href=\"bootstrap.min.css\" rel=\"stylesheet\">
  <link href=\"signin.css\" rel=\"stylesheet\">
</head>
<body class=\"text-center\">
  <form method=\"post\" class=\"form-signin\">
    ");
    
if ($_POST["username"]) {
echo("    <h1 class=\"h3 mb-3 font-weight-normal\" style=\"background-color:red;\"> Hacking Attempt DETECTED! </h1>
    </br> 
  </br>
<img src=\"/static/images/angrypug.jpg\" width=\"300\" height=\"300\">     </br> 
  </br>");

} else {
  echo("      </br>
  <img src=\"/static/images/extrafries.jpeg\" width=\"300\" height=\"300\"> 
    </br> ");
}

echo("
    <label for=\"usernameInput\" class=\"sr-only\">Username</label>
    <input id=\"usernameInput\" name=\"username\" class=\"form-control\" placeholder=\"Username\" required autofocus>
    <label for=\"inputPassword\" class=\"sr-only\">Password</label>
    <input type=\"password\" name=\"password\" id=\"passwordInput\" class=\"form-control\" placeholder=\"Password\" required>
    <button class=\"btn btn-lg btn-primary btn-block\" type=\"submit\">Sign in</button>  
  </form>
</body>
</html>");
} else {
echo("<!doctype html>
<html lang=\"en\">
<head>
  <meta charset=\"utf-8\">
  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\">
  <title>s-q-l</title>
  <link href=\"bootstrap.min.css\" rel=\"stylesheet\">
  </style>
</head>
<body class=\"text-center\">
  <h1 class=\"h3 mb-3 font-weight-normal\">Hello ".$username."!</h1>
  <h3 class=\"h3 mb-3 font-weight-normal\"> Turn on the volume to hear Mario. </h1>

  <audio autoplay>
  <source src=\"/static/audio/masterpiece.mp3\" type=\"audio/ogg\">
  </audio>
  </br> 
  </br>
  <img src=\"/static/images/sticker.webp\" width=\"300\" height=\"300\"> 
    </br> 
  </br></body>
</html>");
}
