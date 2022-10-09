# srdnlenctf 2022

## UNESCO (X solves)

In this challenge we have to retrieve flag hidden in the given image

### Solution

In the picture there is something hidden with steganographic techniques, also because it is very heavy. Using foremost you have 2 zip (one protected with a password and a lot of flag.txt files), 1 wav, 2 png and 4 jpeg. We can start the analysis of all these files using exiftool. In image "23551.jpg" the author field seems interesting, it is an hex string that if decoded you have "ivnuraghe_sunuraxi". Also image "6073.png" has something interesting in copyright field, a base64 string that decoded is "shoulder_machine is the key". According to the just found clues, you have a key and an IV, so maybe there is something related to encryption: infact, if you analyse the unprotected zip, it contains "code.py" which is a decryption code for the flag. The flag must be in the protected zip and it is the one with a different date and a larger size, so "flag2019.txt" but in order to read it, you have to find the password. If you play the audio, in the first seconds there is a DTMF hidden tone and it seems that 4 digits are typed. You can use different online tools to decrypt it or even manually do it as it is very short (4 only digits as you can also see from the spectrum). Once decoded you have 1997, using it as a password for the zip, you can see the content of flag2019.txt. Now you have the encrypted message, the key, the iv and the encryption algorithm from code.py. To retrieve the flag you can put all these elements in the script and run it or use cyberchef (please pay attention to the format of the input: bytes, hex...) and you have the flag.
