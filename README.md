<h1 style="text-align: center;">id3reader</h1>

<h3>About the project</h3>
Project to read the metadata from audio files

Currently supports:
- MP3
- FLAC 

<h3>Guide:</h3>

1. Usage:
  - Call bin/metadata-reader passing path to audio file 

```Shell
bin/metadata-reader <path-to-audio-file>
```

  - The program will offer options such as:
    - printing id3 or stream info
    - printing the audio frame data in the file
    - writing the image to /tmp so that it can be viewed, etc

2. Demo:

  - Viewing lyric metadata
  ![](data/lyric.webp)

  - Viewing song image metadata
  ![](data/image.webp) 



<h3>Fixes:</h3> 
1. Currently for mp3 when printing the frame list, explanation of the frame code has not been added. Users need to check what 
each code means and then print the result
