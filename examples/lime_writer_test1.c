/* Create a LIME test file */
/* Balint Joo 2003 */
/* 12/31/04 C. DeTar added more records to file */

#include <stdio.h>
#include <lime.h>
#include <string.h>
#include <stdlib.h>

int write_rec(LimeWriter *writer, int MB_flag, int ME_flag, char message[], char lime_type[]){
  off_t totbytes = strlen(message);
  off_t bytes;
  LimeRecordHeader *h;
  int status;
  char *bufstart;

  /* Write record header */
  fprintf(stderr, "Creating Header\n");
  h = limeCreateHeader(MB_flag, ME_flag, lime_type, totbytes);
  
  fprintf(stderr, "Writing Header\n");
  status = limeWriteRecordHeader( h, writer );

  if( status < 0 ) { 
    fprintf(stderr, "LIME write header error %d\n", status);
    return EXIT_FAILURE;
  }

  limeDestroyHeader(h);

  /* Write the record in pieces just to test multiple calls */

  fprintf(stderr, "Writing fist part of data\n");
  fflush(stderr);
  bufstart = message;
  bytes = strlen(message)/2;
  status = limeWriteRecordData(bufstart, &bytes, writer);

  if( status < 0 ) { 
    fprintf(stderr, "LIME write error %d\n", status);
    return EXIT_FAILURE;
  }
  fprintf(stderr, "Wrote %d bytes\n", bytes);

  bufstart += bytes;
  bytes = strlen(message)-bytes;
  fprintf(stderr, "Writing second part of data\n");
  fflush(stderr);
  status = limeWriteRecordData(bufstart, &bytes, writer);
  if( status < 0 ) { 
    fprintf(stderr, "LIME write error %d\n", status);
    return EXIT_FAILURE;
  }
  
  fprintf(stderr, "Wrote %d bytes\n", bytes);
}

int main(int argc, char *argv[]) 
{

  char lime_file[] = "lime_file_test";

  LimeWriter *writer;
  FILE *fp;

  /* Open the file for the LimeWriter */
  fprintf(stderr, "Opening file\n");
  fp = fopen(lime_file, "w");
  if(fp == (FILE *)NULL) { 
    fprintf(stderr, "Unable to open %s\n", lime_file);
    return EXIT_FAILURE;
  }

  /* Set up the LimeWriter */
  fprintf(stderr, "Creating Writer\n");
  writer = limeCreateWriter(fp);
  if( writer == (LimeWriter *)NULL ) { 
    fprintf(stderr, "Unable to initialise LIME\n");
    return EXIT_FAILURE;
  }

  /* Write some messages */

  write_rec(writer,1,0,"LIME Rocks Big Time", "lime-test-text1");
  write_rec(writer,0,1,"Go LIME", "lime-test-text2");
  write_rec(writer,1,1,"Whoa dude, like, totally", "lime-test-text3");

  limeDestroyWriter(writer);
  fclose(fp);

  return EXIT_SUCCESS;
}
