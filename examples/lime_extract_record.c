
#include <stdio.h>
#include <stdlib.h>
#include <lime.h>

int main(int argc, char *argv[]) 
{
  char* data_buf;
  
  LimeReader *reader;
  FILE *fp;
  int status;
  size_t nbytes;
  int rec;
  int nrec = 0;
  
  if( argc != 3 ) { 
    fprintf(stderr, "Usage: %s <lime_file> <record number - 0 based>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  fp = fopen(argv[1], "r");
  if(fp == (FILE *)NULL) { 
    fprintf(stderr,"Unable to open file %s for reading\n", argv[1]);
    exit(EXIT_FAILURE);
  }

  rec = atoi(argv[2]);
  if (rec < 0) {
    fprintf(stderr,"Invalid record number = %d\n", rec);
    exit(EXIT_FAILURE);
  }

  reader = limeCreateReader(fp);
  if( reader == (LimeReader *)NULL ) { 
    fprintf(stderr, "Unable to open LimeReader\n");
    exit(EXIT_FAILURE);
  }

  while( (status = limeReaderNextRecord(reader)) != LIME_EOF ){
    
    if( status != LIME_SUCCESS ) { 
      fprintf(stderr, "limeReaderNextRecord returned status = %d\n", 
	      status);
      exit(EXIT_FAILURE);
    }
    
#if 0
    printf("\n\n");
    printf("Type:           %s\n", reader->curr_header->type);
    printf("Data Length:    %d\n", reader->curr_header->data_length);
    printf("Padding Length: %d\n", reader->bytes_pad);
    printf("MB flag:        %d\n", reader->curr_header->MB_flag);
    printf("ME flag:        %d\n", reader->curr_header->ME_flag);
#endif
    
    data_buf = (char *)malloc(sizeof(char)*(reader->bytes_total));
    if( data_buf == (char *)NULL) { 
      fprintf(stderr, "Couldn't malloc data buf\n");
      exit(EXIT_FAILURE);
    }

    nbytes = reader->bytes_total;
    status = limeReaderReadData((void *)data_buf, &nbytes, reader);

    if( status < 0 ) { 
      if( status != LIME_EOR ) { 
	fprintf(stderr, "LIME Read Error Occurred: status= %d  %d bytes wanted, %d read\n", status,
		reader->bytes_total, nbytes);
	exit(EXIT_FAILURE);
      }
    }

    /* Print it to stdout if this is the desired record */
    if (rec == nrec)
    {
      fwrite(data_buf, nbytes, 1, stdout);
      free(data_buf);
      break;
    }

    free(data_buf);
    nrec++;
  }

  limeDestroyReader(reader);
  fclose(fp);

  exit(0);
}   
    
  
