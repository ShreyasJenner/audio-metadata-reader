#ifndef JPEG_WRITER_H
#define JPEG_WRITER_H

#include "audio-metadata-reader/log.h"
#include "audio-metadata-reader/stdheader.h"

#define MP3_IMAGE "/tmp/mp3image.img"

int check_JPEG(uint8_t *data, int buffer_size);

int jpeg_writer(uint8_t *data, int buff_size);

#endif
