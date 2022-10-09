# srdnlenctf 2022

## Osint 1 (X solves)

The challenge gives us a picture taken like from a mountain and the goal is to find the coordinates (up to the 4th decimal digit) of the place where the photo has been taken.

### Solution

Using exiftool to see the metadata there is a GPS position info but it is the wrong one. If we load the image into Google search by Images we have a lot of similar pictures with titles "Poetto", "Sella del Diavolo", "Marina Piccola", so it is about a beach near Cagliari.
To discover the exact coordinates we can look in Google Maps to pictures taken in the above places and pictures of "Sella del Diavolo" are the most similar ones and looking at "Street View and spherical photos", the second one has the same view. With this option we have in the below part of the screen the point of the picture and in the URL the coordinates: 39.1893774,9.1619935. This are the wrong ones because the picture has not been taken from this exact point, so coming back to the classical view of maps you have to mix all elements (for example the fact that it must be taken in the trail, near the harbour) and find the coordinates 39.1888,9.1639.
