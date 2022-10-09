# srdnlenctf 2022

## Network Exfiltration (X solves)

We have a pcap of a USB traffic and analyzing it we have to retrieve the flag.

### Solution

Data found in usb.capdata (Leftover Capture Data) and analyzing deeply the content of that column, in the first packet we can read JFIF and in other packets jpg. We can understand that this USB traffic is about a transfer file and we have to recover them with the following steps:
```bash 
tshark -r tdk_challenge.pcap -Y usb.capdata -T field -e usb.capdata > raw
```
We redirect all the content of our pcap in the capdata section into a file and
```bash
xxd -r -p raw raw.bin
```
With the previous command we convert the file into a binary one
And the last one to extract hidden data from the binary
```
binwalk --dd='.*' raw.bin 
```
Now, inspect all the extracted images in the directory and we find a QR code picture, scanning it we have the flag.
