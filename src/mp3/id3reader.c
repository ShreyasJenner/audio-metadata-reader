#include "audio-metadata-reader/mp3/id3reader.h"
#include "audio-metadata-reader/mp3/mp3_lut.h"

/*
 * Check if id3 frame header exists at file descriptor position
 * function resets fd back to initial position it was in when passed to this
 * function
 */
int id3_framecheck(int fd) {
  int i;
  char id[4];

  read(fd, id, 4);

  /* check if the 1st 4 bytes can be an id3 frame header id */
  for (i = 0; i < 4; i++) {
    if (!(id[i] >= 'A' && id[i] <= 'Z') && !(id[i] >= '0' && id[i] <= '9')) {
      lseek(fd, -4, SEEK_CUR);
      return 0;
    }
  }
  lseek(fd, -4, SEEK_CUR);
  return 1;
}

/*
 * Function to get number of frames in id3 tag
 * Assumes that fd is pointing to the start of the first frame
 * Sets fd to the position it was in when passed to the function
 */
int get_id3framecount(int fd) {
  int cntr, pos;
  ID3FrameHeader *hdr;

  /* Initialization */
  cntr = 0;
  pos = lseek(fd, 0, SEEK_CUR);

  /* loop will keep running until frames run out */
  while (true) {
    /* get id3 frame header
     * if it doesnt exist, then break the loop
     */
    hdr = get_id3frameheader(fd);
    if (hdr == NULL)
      break;

    /* set fd to end of frame and increment counter */
    lseek(fd, hdr->frame_size, SEEK_CUR);
    cntr++;
  }

  /* set fd to original position */
  lseek(fd, pos, SEEK_SET);

  /* store cntr in struct and return struct ptr */
  return cntr;
}

/*
 * Function to get list of frames in id3 tag
 * Assumes that fd is pointing to the start of the first frame
 * Sets fd to the position it was in when passed to the function
 */
char **get_id3framelist(int fd, int count) {
  int cntr, pos;
  ID3FrameHeader *hdr;
  char **frame_list;

  /* Initialization */
  cntr = 0;
  pos = lseek(fd, 0, SEEK_CUR);
  frame_list = (char **)malloc(sizeof(char *) * count);

  /* loop will keep running until frames run out */
  while (true) {
    /* get id3 frame header
     * if it doesnt exist, then break the loop
     */
    hdr = get_id3frameheader(fd);
    if (hdr == NULL)
      break;

    /* allocate space for frame id and store */
    frame_list[cntr] = (char *)malloc(sizeof(hdr->frame_id));
    strcpy(frame_list[cntr++], hdr->frame_id);

    /* set fd to end of frame */
    lseek(fd, hdr->frame_size, SEEK_CUR);
    free(hdr);
  }

  /* set fd to original position */
  lseek(fd, pos, SEEK_SET);

  /* store ptr to */
  return frame_list;
}

/*
 * Function prints an id3 frame header information onto terminal
 */
void show_id3frameheader(ID3FrameHeader *hdr) {
  printf("Frame ID: %s\n", hdr->frame_id);
  printf("Frame Size: %d\n", hdr->frame_size);
  printf("Tag alter preservation: %s\n",
         hdr->flags[0] & 64u >> 6 ? "Discard" : "Preserve");
  printf("File alter preservation: %s\n",
         hdr->flags[0] & 32u >> 5 ? "Discard" : "Preserve");
  printf("Read Only: %s\n", hdr->flags[0] & 16u >> 4 ? "True" : "False");
  printf("Group Frame: %s\n", hdr->flags[1] & 64u >> 6 ? "True" : "False");
  printf("Compression: %s\n", hdr->flags[1] & 8u >> 3 ? "True" : "False");
  printf("Encryption: %s\n", hdr->flags[1] & 4u >> 2 ? "True" : "False");
  printf("Unsynchronization: %s\n", hdr->flags[1] & 2u >> 1 ? "True" : "False");
  printf("Data length indicator: %s\n", hdr->flags[1] & 1u >> 0 ? "Yes" : "No");
}

/*
 * Function stores id3 frame header into a struct and returns it
 * assumes fd is passed to it with its position at frame header
 */
ID3FrameHeader *get_id3frameheader(int fd) {
  /* check if frame exists; return NULL if it doesn't */
  if (id3_framecheck(fd) == 0)
    return NULL;

  char frame_id[4];
  uint32_t size;
  uint8_t flags[2];
  ID3FrameHeader *hdr;

  /* allocate space for struct */
  hdr = malloc(sizeof(ID3FrameHeader));

  /* read 10 bytes from file */
  read(fd, frame_id, 4);
  read(fd, &size, 4);
  read(fd, flags, 2);

  /* change endianess of var `size` to big endian */
  size = __bswap_constant_32(size);

  /* print data */
  strcpy(hdr->frame_id, frame_id);
  hdr->frame_size = sync_safe_int_to_int(size);
  hdr->flags[0] = flags[0] & 64u >> 6;
  hdr->flags[1] = flags[0] & 32u >> 5;
  hdr->flags[2] = flags[0] & 16u >> 4;
  hdr->flags[3] = flags[1] & 64u >> 6;
  hdr->flags[4] = flags[1] & 8u >> 3;
  hdr->flags[5] = flags[1] & 4u >> 2;
  hdr->flags[6] = flags[1] & 2u >> 1;
  hdr->flags[7] = flags[1] & 1u >> 0;

  return hdr;
}

/*
 * Function returns a pointer to an id3 frame
 */
ID3Frame *get_id3frame(int fd) {
  ID3Frame *frame;
  uint8_t *data;

  /* allocate space for frame */
  frame = malloc(sizeof(ID3Frame));

  /* if frame header is null then return null */
  frame->hdr = get_id3frameheader(fd);
  if (frame->hdr == NULL) {
    free(frame);
    return NULL;
  }

  /* allocate space for frame data */
  data = malloc(frame->hdr->frame_size);
  frame->data = data;

  /* read data into frame data field */
  read(fd, frame->data, frame->hdr->frame_size);

  return frame;
}

/*
 * Function is passed path to mp3 file
 * Checks if file contains id3 tag
 * Restores position of file pointer after reading
 * Returns 1 on success
 * Returns 0 on failure
 */
int id3_tagcheck(char *filename) {
  /* Declaration */
  int fd;
  char tag[3];

  /* Open file */
  fd = open(filename, O_RDONLY);

  /* read file */
  read(fd, tag, 3);
  lseek(fd, -3, SEEK_CUR);

  /* close file */
  close(fd);

  return tag[0] == 'I' && tag[1] == 'D' && tag[2] == '3';
}

/*
 * Function gets id3 tag header info and stores in struct pointer
 * The
 */
ID3TagHeader *get_id3tagheader(int fd, ID3TagHeader *hdr) {
  char identifier[3];
  uint8_t version[2];
  uint8_t flags;
  uint32_t size;

  /* allocate space for tag header */
  hdr = malloc(sizeof(ID3TagHeader));

  /* read data from id3 header */
  read(fd, identifier, 3);
  read(fd, version, 2);
  read(fd, &flags, 1);
  read(fd, &size, 4);

  /* change endianess of var `size` to big endian */
  size = __bswap_constant_32(size);

  /* fill struct */
  strcpy(hdr->identifier, identifier);
  hdr->version[0] = version[0];
  hdr->version[1] = version[1];
  hdr->flags[0] = (flags & 8u) >> 7;
  hdr->flags[1] = (flags & 7u) >> 6;
  hdr->flags[2] = (flags & 6u) >> 5;
  hdr->flags[3] = (flags & 5u) >> 4;
  hdr->size = sync_safe_int_to_int(size);

  return hdr;
}

/*
 * print the id3 tag header onto the terminal
 */
void show_id3tagheader(ID3Tag *tag) {
  printf("Identifier: %s\n", tag->hdr->identifier);
  printf("Major Version: %d\n", tag->hdr->version[0]);
  printf("Revision No: %d\n", tag->hdr->version[1]);
  printf("Unsynchronization: %s\n",
         (tag->hdr->flags[0] & 8u) >> 7 ? "True" : "False");
  printf("Extended Header: %s\n",
         (tag->hdr->flags[1] & 7u) >> 6 ? "True" : "False");
  printf("Experimental Indicator: %s\n",
         (tag->hdr->flags[2] & 6u) >> 5 ? "True" : "False");
  printf("Footer: %s\n", (tag->hdr->flags[3] & 5u) >> 4 ? "True" : "False");
  printf("Size: %d/%02x\n", tag->hdr->size, tag->hdr->size);
}

/* Function to free id3 tag */
void ID3_FREE(ID3Tag *tag) {
  int i;

  for (i = 0; i < tag->frame_no; i++) {
    free(tag->frame_arr[i]->hdr);
    free(tag->frame_arr[i]->data);
    free(tag->frame_arr[i]);
    free(tag->frame_list[i]);
  }

  free(tag->frame_list);
  free(tag->frame_arr);
  free(tag->hdr);
  free(tag);
}

/*
 * Function is passed path to an mp3 file
 * Return a struct containing all id3 information if id3 tag is present
 * Return NULL if no id3 tag
 */
ID3Tag *get_id3tag(char *filename) {
  int err, i, fd;
  ID3Tag *tag;

  /* check for id3 tag */
  err = id3_tagcheck(filename);
  if (err == 0) {
    return NULL;
  }

  /* open file */
  fd = open(filename, O_RDONLY);
  if (fd < 0) {
    logerror(__FILE__, __LINE__, __func__, "Error opening file");
    return NULL;
  }

  /* allocate space for tag struct */
  tag = malloc(sizeof(ID3Tag));

  /* get tag header */
  tag->hdr = get_id3tagheader(fd, tag->hdr);

  /* get number of frames and and list of frames */
  tag->frame_no = get_id3framecount(fd);
  tag->frame_list = get_id3framelist(fd, tag->frame_no);
  tag->frame_arr = malloc(sizeof(ID3Frame *) * tag->frame_no);

  /* calculate tag size */
  tag->size = tag->hdr->size + 10 + (10 * tag->hdr->flags[3]);

  /* get frames */
  for (i = 0; i < tag->frame_no; i++)
    tag->frame_arr[i] = get_id3frame(fd);

  /* reposition file descriptor */
  lseek(fd, 0, SEEK_SET);

  return tag;
}
