#include <stdio.h>
#include <lime.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) 
{

  char *message = "Lime Rocks Big Time";

  LimeWriter *dg;
  LimeRecordHeader *h;
  int status;
  size_t bytes;
  char *bufstart;
  FILE *fp;

  /* Open the file for the LimeWriter */
  fprintf(stderr, "Opening file\n");
  fp = fopen("lime_output.bin", "w");
  if(fp == (FILE *)NULL) { 
    fprintf(stderr, "Unable to open output.bin\n");
    exit(EXIT_FAILURE);
  }

  /* Set up the LimeWriter */
  fprintf(stderr, "Creating Writer\n");
  dg = limeCreateWriter(fp);
  if( dg == (LimeWriter *)NULL ) { 
    fprintf(stderr, "Unable to initialise LIME\n");
    exit(EXIT_FAILURE);
  }

  /* LimeOutput the message */
  fprintf(stderr, "Creating Header\n");
  h = limeCreateHeader(1,1,
		       "application/text", 
		       strlen(message));

  
  fprintf(stderr, "Writing Header\n");
  status = limeWriteRecordHeader( h, 
				  1, /* First and Last record */
				  dg );

  if( status < 0 ) { 
    fprintf(stderr, "Oh dear some horrible error has occurred. status is: %d\n", status);
    exit(EXIT_FAILURE);
  }

  limeDestroyHeader(h);

  /* Write the record */
  fprintf(stderr, "Writing fist part of data\n");
  fflush(stderr);
  bufstart = message;
  bytes = strlen(message)-5;
  status = limeWriteRecordData(bufstart, &bytes, dg);

  if( status < 0 ) { 
     fprintf(stderr, "Oh dear some horrible error has occurred. status is: %d\n", status);
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "Wrote %d bytes\n", bytes);

  bufstart += bytes;
  bytes = strlen(message)-bytes;
  fprintf(stderr, "Writing second part of data\n");
  fflush(stderr);
  status = limeWriteRecordData(bufstart, &bytes, dg);
  if( status < 0 ) { 
     fprintf(stderr, "Oh dear some horrible error has occurred. status is: %d\n", status);
    exit(EXIT_FAILURE);
  }
  
  fprintf(stderr, "Wrote %d bytes\n", bytes);
  limeDestroyWriter(dg);
  fclose(fp);
}
