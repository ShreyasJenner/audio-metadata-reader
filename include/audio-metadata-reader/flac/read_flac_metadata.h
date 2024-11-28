#ifndef READ_FLAC_METADATA
#define READ_FLAC_METADATA

#define FLAC_IMAGE "/tmp/flacimage.img"

#include "audio-metadata-reader/log.h"
#include "audio-metadata-reader/stdheader.h"

typedef struct flac_metadata FLACMetadata;

/* stream info metadata block */
struct flac_metadata {
  FLAC__StreamMetadata *streaminfo;
  FLAC__StreamMetadata *padding;
  FLAC__StreamMetadata *app;
  FLAC__StreamMetadata *seektable;
  FLAC__StreamMetadata *vorbis_comment;
  FLAC__StreamMetadata *cue_sheet;
  FLAC__StreamMetadata *picture;
};

int flac_check(char *filename);

FLACMetadata *allocate_FLACMetadataSpace();

void store_FLACMetadata(FLACMetadata *metadata, FLAC__MetadataType metatype,
                        FLAC__StreamMetadata *streammetadata);

void view_FLACMetadata(char *filename);

FLACMetadata *get_FLACMetadata(char *filename);

void clean_FLACMetadata(FLACMetadata *metadata);

#endif // #ifndef READ_FLAC_METADATA
