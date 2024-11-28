#ifndef MP3_LUT_H
#define MP3_LUT_H

#include "audio-metadata-reader/mp3/id3_structs.h"
#include "audio-metadata-reader/stdheader.h"

int verify_mp3Header(uint8_t *bytes);

void show_mp3FrameHeader(uint8_t *bytes);

#endif
