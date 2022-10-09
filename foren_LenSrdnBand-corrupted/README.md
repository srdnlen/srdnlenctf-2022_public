# srdnlenctf 2022

## LenSrdnBand-corrupted (X solves)

In this challenge we have a corrupted PNG in the header and chunk names. The flag is hidden in the LSB of the PNG.

### Solution

The name of the file tells us that the file is corrupt. Using HxD we can see how the header is almost the same as png.

Correcting the header the file will be recognized as an image but still not enough. Using pngcheck we can see that the image contains an unknown chunk called ITTT.
This can mean that the name of the chunk is wrong and among the various possible chunks the most plausible one is the IDAT one, i.e. the chunk of the image data. This can mean that the name of the chunk is wrong and among the various possible chunks the most plausible one is the IDAT one, i.e. the chunk of the image data. By replacing all the ITTT chunks with IDAT we get an uncorrupted image.

In the end, looking by reading only the capital letters in the title we can imagine that the flag is in the LSB, and looking at the plane Red Zero we can find the flag.
