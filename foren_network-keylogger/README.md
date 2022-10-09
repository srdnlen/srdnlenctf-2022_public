# srdnlenctf 2022

## Network Keylogger (X solves)

In this challenge we have a USB captured data and as written in the filename it is about keyboard.

### Solution

The confirmation that this capture is about a keyboard can be found in the first packets where every device leaves a trace. Data of the USB keyboard can be found in the Leftover Capture Data (usb.capdata), so we export this with the following command:

```bash
tshark -r keyboard.pcap -T fields -e usb.capdata -Y usb.capdata | grep -v 0000000000000000 > data.txt
```

we also delete empty bytes with the grep instruction.
After this we have to map the hex bytes into ASCII characters. You can do this by hand or with a script but to translate it you have to take in count first byte which means the KEY MOD (like ctrl, alt, shift, meta) and the third byte which means the pressed key (letters and punctuation, special characters). To correctly identify the key, you can use the official library https://gist.github.com/MightyPork/6da26e382a7ad91b5496ee55fdc73db2#file-usb_hid_keys-h
After all this steps, you will have a decoded message with the flag.
