#include "lime.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

#include "lime_binary_header.h"
#include "lime_utils.h"


/* This is suppose to be the standard prototype for fseeko */
int fseeko(FILE *stream, off_t offset, int whence);


/* Forward declaration */
int write_lime_record_binary_header(FILE *fp, LimeRecordHeader *h);
int skip_lime_record_binary_header(FILE *fp);
int skipWriterBytes(LimeWriter *w, size_t bytes_to_skip);

LimeWriter* limeCreateWriter(FILE *fp)
{
  LimeWriter* ret_val;

#ifdef DEBUG
  fprintf(stderr, "Initialising LIME Generator\n");
#endif
  ret_val = (LimeWriter *)malloc(sizeof(LimeWriter));
  if( ret_val == (LimeWriter *)NULL ) { 
    return NULL;
  }

  ret_val->fp = fp;
  ret_val->isLastP = 1;
  ret_val->first_record = 1;
  ret_val->last_written = 0;
  ret_val->header_nextP = 1;
  ret_val->bytes_left = 0;
  ret_val->bytes_total = 0;
  ret_val->bytes_pad = 0;
  return ret_val;
}

int limeDestroyWriter(LimeWriter *s)
{

  LimeRecordHeader *h;
  int status;
  size_t nbytes = 0;
#ifdef DEBUG
  fprintf(stderr, "Closing Lime Generator\n");
#endif

  if( s->last_written != 1 ) { 

#ifdef DEBUG
    fprintf(stderr, "Writing empty last record\n");
#endif
    /* Writing a last empty record */
    h = limeCreateHeader(0, 0,
			 "", 
			 0);

    if( h == (LimeRecordHeader *)NULL ) { 
      fprintf(stderr, "Unable to close LIME\n");
      exit(EXIT_FAILURE);
    }

    limeWriteRecordHeader(h, 1, s);
    nbytes = 0;
    limeWriteRecordData(NULL,&nbytes,s); 
    limeDestroyHeader(h);
  }

  free(s);
  return LIME_SUCCESS;
}


int limeWriteRecordHeader( LimeRecordHeader *props, int do_write,
			   LimeWriter *d)
{
  /* If do_write is false we set state as though we wrote the header,
     but don't actually write it */

  int ret_val;

#ifdef DEBUG 
  fprintf(stderr, "In limeWriteRecordHeader\n");
  fflush(stderr);
#endif

  if( d == (LimeWriter *)NULL ) { 
#ifdef DEBUG
    fprintf(stderr, "d is NULL\n");
    fflush(stderr);
#endif
    return LIME_ERR_PARAM;
  }

  if( props == (LimeRecordHeader *)NULL ) { 
#ifdef DEBUG
    fprintf(stderr, "props is NULL\n");
    fflush(stderr);
#endif
    return LIME_ERR_PARAM;
  }

  /* Some consistency checks ... */

  /* Make sure header is expected now */
  if( d->header_nextP != 1 ) { 
    return LIME_ERR_HEADER_NEXT;
  }

  /* If last record ended a message, this one must begin a new one */
  /* If last record did not end a message, this one must not begin one */
  if(  d->isLastP != props->MB_flag )
    return LIME_ERR_MBME;

  /* Write header, if requested, or advance file pointer past header */
  /* All node file pointers should then be positioned after the header */
  if(do_write)
    ret_val = write_lime_record_binary_header(d->fp, props);
  else
    ret_val = skip_lime_record_binary_header(d->fp);

  /* Set new writer state */
  d->isLastP      = props->ME_flag;
  d->first_record = props->MB_flag;
  d->bytes_left   = props->data_length;
  d->bytes_total  = props->data_length;
  d->bytes_pad    = lime_padding(props->data_length);
  d->header_nextP = 0;

  return ret_val;
 
}

int limeWriteRecordData( void *source, size_t *nbytes,
			    LimeWriter* d)
{
  size_t bytes_to_write;
  size_t ret_val;
  unsigned char padbuf[7] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  int pad;

#ifdef DEBUG
  fprintf(stderr, "In LimeWriteRecordData\n");
#endif

  if ( d->header_nextP == 1 ) { 
    *nbytes=0;
    return LIME_ERR_PARAM;
  }

  if( *nbytes > 0 ) {
    /* If we want to write more than there is room for in the 
       current record -- then we simply truncate to how much
       there is still room for. We adjust the nbytes for return
       accordingly */
    if (*nbytes > d->bytes_left ) { 
      bytes_to_write = d->bytes_left;
    }
    else {
      bytes_to_write = *nbytes;
    }
    
    /* Try to write so many bytes */
    ret_val = fwrite((const void *)source, sizeof(unsigned char), 
		     bytes_to_write, d->fp);
    
    *nbytes = ret_val;
    
    if( ret_val != bytes_to_write ) { 
      return LIME_ERR_WRITE;
    }

    /* We succeeded */
    d->bytes_left -= bytes_to_write;
  
  }

  if( d->bytes_left == 0 ) { 
    /* Stuff to do here */
    /* Padding */
    pad = lime_padding(d->bytes_total);
    if( pad > 0 ) { 
      ret_val = fwrite((const void *)padbuf, sizeof(unsigned char),
		       pad, d->fp);

      if( ret_val != pad ) { 
	return LIME_ERR_WRITE;
      }
    }

    d->header_nextP = 1;  /* Next thing to come is a header */
    
    if( d->isLastP == 1 ) {
      d->last_written = 1;
    }
  }

  return LIME_SUCCESS;
}
  
/* Advance to end of current record */
/* We need this to close out a record when we have more than one node
   writing to the same file */

int limeWriterCloseRecord(LimeWriter *w)
{
  printf("limeWriterCloseRecord not implemented\n");
  return 0;
}
  

int skip_lime_record_binary_header(FILE *fp)
{
  return fseeko(fp, (off_t)LIME_HDR_LENGTH, SEEK_CUR);
}


int write_lime_record_binary_header(FILE *fp, LimeRecordHeader *h)
{

  int i;
  int ret_val;

#ifdef DEBUG
  fprintf(stderr, "In write_lime_record_binary_header\n");
#endif

  /* Clear header */
  for(i = 0; i < MAX_HDR64; i++)lime_header.int64[i] = 0;

  /* Load values, converting integers to big endian if needed */
  *lime_hdr_magic_no = big_endian_long((n_uint32_t)LIME_MAGIC_NO);
  *lime_hdr_version  = big_endian_short((n_uint16_t)h->lime_version);

  /* MB flag. */
  if( h->MB_flag == 1 ) { 
    *lime_hdr_mbme = ( *lime_hdr_mbme | MB_MASK );
  }

  /* ME flag */
  if( h->ME_flag == 1 ) { 
    *lime_hdr_mbme = ( *lime_hdr_mbme | ME_MASK );
  }

  /* Data length */
  *lime_hdr_data_len = 
    big_endian_long_long((n_uint64_t)h->data_length);

  /* Record type string - trailing nulls  */
  strncpy((char*)lime_hdr_rec_type,h->type,MAX_LIME_HDR_REC_TYPE);

  /* Force a null termination */
  lime_hdr_rec_type[MAX_LIME_HDR_REC_TYPE] = '\0';

  /* Write the header */
  ret_val =fwrite((const void *)lime_header.int64, 
		  sizeof(n_uint64_t), MAX_HDR64, fp);

  if( ret_val < MAX_HDR64 ) { 
    return LIME_ERR_WRITE;
  }

  return LIME_SUCCESS;
}  

/* Skip bytes within the current record.  If the skip takes us to the
   end of the data, skip any padding as well and set the end of record
   flag.  If the skip takes us past the end of the data in the current
   record, skip to end of the record and return an error. */

int skipWriterBytes(LimeWriter *w, size_t bytes_to_skip)
{
  unsigned char *buf;

  int status = LIME_SUCCESS;
  size_t bytes_to_seek;

  /* Ignore zero.  No negative skips */
  if(bytes_to_skip == 0)return LIME_SUCCESS;
  if(bytes_to_skip < 0)return LIME_ERR_SEEK;

  /* Prevent skip past the end of the data */
  if(w->bytes_left < bytes_to_skip){
    bytes_to_skip = w->bytes_left;
    printf("Seeking past end of data\n");
    status = LIME_ERR_SEEK;
  }

  /* If there will be no bytes left, include padding in the seek */
  bytes_to_seek = bytes_to_skip;
  if(w->bytes_left == bytes_to_skip)
    bytes_to_seek += w->bytes_pad;
    
  /* Seek */
  if(bytes_to_seek > 0){
    status = fseeko(w->fp, (off_t)bytes_to_seek, SEEK_CUR);
    if(status < 0){
      return LIME_ERR_SEEK;
      printf("fseek returned %d\n");
    }
  }

  /* Update the writer state */
  w->bytes_left -= bytes_to_skip;
  
  return status;
}

int limeWriterSeek(LimeWriter *r, off_t offset, int whence){
  fprintf(stderr, "limeWriterSeek not implemented yet\n");
  return LIME_ERR_WRITE;
}
