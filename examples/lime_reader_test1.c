
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
  
  if( argc != 2 ) { 
    fprintf(stderr, "Usage: %s <lime_file>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  

  fp = fopen(argv[1], "r");
  if(fp == (FILE *)NULL) { 
    fprintf(stderr,"Unable to open file %s for reading\n", argv[1]);
    exit(EXIT_FAILURE);
  }

  reader = limeCreateReader(fp);
  if( reader == (LimeReader *)NULL ) { 
    fprintf(stderr, "Unable to open LimeReader\n");
    exit(EXIT_FAILURE);
  }

  while( reader->is_last != 1 ) {
    status = limeReaderNextRecord(reader);

    if( status != LIME_SUCCESS ) { 
      fprintf(stderr, "limeReaderNextRecord returned status = %d\n", 
	      status);
      exit(EXIT_FAILURE);
    }
    
    if( reader->curr_header->typet == TYPE_UNCHANGED ) {
      printf("Continuation of chunk\n");
    }
     
    printf("Type T: %d\n",  reader->curr_header->typet);
    printf("Type  : %s\n",  reader->curr_header->type);
    printf("ID Length: %d\n", reader->curr_header->id_length);
    printf("ID  : %s\n",  reader->curr_header->id);
    printf("Data Length:%d\n", reader->bytes_total);
    printf("ME flag: %d\n", reader->curr_header->ME_flag);
    
    data_buf = (char *)malloc(sizeof(char)*(reader->bytes_total)+1);
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

    data_buf[reader->bytes_total]='\0';
    printf("Data : \"%s\" \n", data_buf);
    free(data_buf);

  }

  limeDestroyReader(reader);
  fclose(fp);

}   
    
  
