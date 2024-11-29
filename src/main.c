#include "audio-metadata-reader/flac/read_flac_metadata.h"
#include "audio-metadata-reader/images/jpeg.h"
#include "audio-metadata-reader/log.h"
#include "audio-metadata-reader/mp3/id3_structs.h"
#include "audio-metadata-reader/mp3/id3reader.h"
#include "audio-metadata-reader/mp3/mp3_lut.h"
#include "audio-metadata-reader/stdheader.h"
#include "audio-metadata-reader/syncint.h"
#include "driver.h"

/* Driver Tag */
int main(int argc, char **argv) {
  /* check if file arg passed */
  if (argc < 2) {
    logerror(__FILE__, __LINE__, __func__, "Argument missing");
    exit(1);
  }

  /* check if passed file is mp3 */
  if (id3_tagcheck(argv[1])) {
    printf("mp3 file detected\n");
    program_demo(argv[1]);
  } else if (flac_check(argv[1])) {
    printf("Flac file identified\n");
    view_FLACMetadata(argv[1]);
  } else {
    printf("Unkown file format\n");
  }

  return 0;
}
