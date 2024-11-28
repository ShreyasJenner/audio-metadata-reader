#ifndef ID3READER_H
#define ID3READER_H

#include "audio-metadata-reader/images/jpeg.h"
#include "audio-metadata-reader/log.h"
#include "audio-metadata-reader/mp3/id3_structs.h"
#include "audio-metadata-reader/stdheader.h"
#include "audio-metadata-reader/syncint.h"

int id3_framecheck(int fd);

void show_id3frameheader(ID3FrameHeader *hdr);

ID3FrameHeader *get_id3frameheader(int fd);

int get_id3framecount(int fd);

char **get_id3framelist(int fd, int count);

ID3Frame *get_id3frame(int fd);

int id3_tagcheck(char *filename);

ID3TagHeader *get_id3tagheader(int fd, ID3TagHeader *hdr);

void show_id3tagheader(ID3Tag *tag);

ID3Tag *get_id3tag(char *filename);

void id3_View(char *filename);

void ID3_FREE(ID3Tag *tag);

#endif
