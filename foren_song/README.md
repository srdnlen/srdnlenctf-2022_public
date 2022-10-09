# srdnlenctf 2022

## Song (X solves)

It seems that we have to retrieve something hidden in this wav file.

### Solution

Most of the times, when there is something hidden in audio, you have to use specific software like Sonic Visualizer, Audacity. If you play the audio you immediately hear in the first seconds an interference. Loading the audio into Audacity and looking at the spectrum with specific zoom settings, you can read in the first seconds "hint" and moving on the length of the audio, you see "hint ita wiki dimonios", which means that you have to find "dimonios" into wikipedia in italian. This page leads to 2 results, one about a ferry and the other called "Inno della Brigata Sassari". If you have used apps like Shazam to recognize the song, it is the official song of the military called "Brigata Sassari", so the first result on wikipedia will be useful. Reading it there is no flag but in the description it says to travel in the time, which refers to the wayback machine an instrument able to see backup of pages and so retrieve deleted content. In wayback machine, at the bottom of the page, there is a false flag saying to see the main page ("Voce principale"), unfortunately the page is only in italian. The main page of this one, is "Brigata meccanizzata Sassari" that does not contain flags but if you put it into wayback machine, there is a snapshot on 05/09/2022 at 12:20:23 with the flag in the notes.
