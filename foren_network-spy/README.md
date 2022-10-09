# srdnlenctf 2022

## Network Spy (X solves)

We have a USB capture and as the name of the file, it is related to a mouse.

### Solution

Analysing the first packets we can see a Trust one but the idProduct is not known. To analyze the captured data, we have to inspect usb.capdata but we have to filter the extracted bytes: we have to cut first byte "03" which stands for the device id. The strip part can be done witht the following script but first of all save them in a proper file with the command

```bash
tshark -r mouse.pcap -T fields -e usb.capdata -Y usb.capdata > mouse.txt"
```

And to strip the data:

```python
#!/bin/env python3
with open("mouse.txt", "r+") as f:
    a = f.readlines()

with open("mouse_stripped.txt", "w") as f: #with this code we add the semicolon every 2 digit, otherwise gawk function is not able to compute
    for j in range(len(a)):
        line = a[j]
        n=2
        l = [line[i:i+n] for i in range(0, len(line), n)]
        s = "\n"
        for i, el in enumerate(l):
            if i != len(l)-1:
                s+= el
                s += ":"


        s = s[4:-1]
        #we select only from the fifth digit because 0300 (the first bytes) is about the peripheral, not about coordinates
        print(s)
        f.write(s)
        f.write("\n")
```

Now we have to convert the stripped data into coordinates in order to paint the points and retrieve the message. This can be done with the gawk function as follows:

```bash
gawk -F: 'function comp(v){if(v>127)v-=256;return v}{x+=comp(strtonum("0x"$2));y+=comp(strtonum("0x"$3))}$1=="01"{print x,y}' mouse_stripped.txt > coords.txt
```

Now we only have to plot the coordinates with gnuplot. First, open gnuplot with gnuplot and in that shell type "gnuplot coords.txt". An image is shown and by mirroring the photo you can read the flag.
