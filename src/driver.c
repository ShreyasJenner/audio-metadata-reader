/* Program file contains a menu driven code that prints id3 data
 * and  MP3 frame header data onto the terminal */

#include "driver.h"

/* Function to view id3 data */
void program_demo(char *filename) {
  /* Declaration */
  int choice, i, flag;
  ID3Tag *tag;
  MP3FrameHeader *mfhd;

  /* get the id3 tag and store it */
  tag = get_id3tag(filename);
  if (tag == NULL) {
    logerror(__FILE__, __LINE__, __func__, "No ID3 Data");
    exit(1);
  }

  // get the MP3FrameHeader struct
  mfhd = get_mp3FrameHeader(filename);

  flag = 1;
  choice = 'h';
  while (flag) {
    switch (choice) {
    case 'h':
      printf("Available information:\n");
      printf("\t1:ID3 header information\n");
      printf("\t2:Size of ID3 tag\n");
      printf("\t3:No of ID3 frames\n");
      printf("\t4:ID3 frame list\n");
      printf("\t5:ID3 frame header information\n");
      printf("\t6:Details of ID3 frames\n");
      printf("\t7: MP3 Frame Header details\n");
      printf("\th:Help\n");
      break;

    case 'q':
      printf("Exiting Program\n");
      flag = 0;
      break;

    case 1:
      show_id3tagheader(tag);
      break;
    case 2:
      printf("%d/%02x\n", tag->size, tag->size);
      break;
    case 3:
      printf("%d\n", tag->frame_no);
      break;
    case 4:
      for (i = 0; i < tag->frame_no; i++)
        printf("%d:%s\n", i, tag->frame_list[i]);
      break;
    case 5:
      printf("Enter frame number:");
      if (scanf("%d", &choice) != 1) {
        choice = fgetc(stdin);
      }

      if (choice < tag->frame_no)
        show_id3frameheader(tag->frame_arr[choice]->hdr);
      else
        printf("Out of range\n");
      break;
    case 6:
      printf("Enter frame number:");
      if (scanf("%d", &choice) != 1) {
        choice = fgetc(stdin);
      }
      if (choice < tag->frame_no) {

        // if APIC wasn't called directly print frame details onto terminal
        if (strcmp(tag->frame_list[choice], "APIC")) {
          printf("%s:%s\n", tag->frame_list[choice],
                 tag->frame_arr[choice]->data);

          /* write image to /tmp file */
        } else {

          // TODO: add support for other image formats
          if (check_JPEG(tag->frame_arr[choice]->data,
                         tag->frame_arr[choice]->hdr->frame_size) == -1) {
            printf("Only JPEG image is currently supported\n");
          } else {
            jpeg_writer(tag->frame_arr[choice]->data,
                        tag->frame_arr[choice]->hdr->frame_size);
          }
        }
      } else {
        printf("Out of range\n");
      }
      break;

      // print the mp3 frame header details onto the terminal
    case 7:
      show_mp3FrameHeader(mfhd);
      break;

    default:
      break;
    }

    if (flag && scanf("%d", &choice) != 1) {
      choice = fgetc(stdin);
    }
  }

  /* clean up resources */
  ID3_FREE(tag);
  MP3FrameHeader_FREE(mfhd);
}
