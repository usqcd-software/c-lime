#include <lime.h>
#include <stdio.h>
#include <stdlib.h>

#include <lime_utils.h>
#include <lime_binary_header.h>

#undef LIME_DEBUG

/* This is supposed to be the standard prototype for fseeko */
int fseeko(FILE *stream, off_t offset, int whence);

/* Forward declarations for internal routines */
int skipReaderBytes(LimeReader *r, size_t bytes_to_skip);
int readAndParseHeader(LimeReader *r);

LimeReader* limeCreateReader(FILE *fp)
{
  LimeReader *ret_val;

  ret_val = (LimeReader *)malloc(sizeof(LimeReader));
  if( ret_val == (LimeReader *)NULL ) { 
    return NULL;
  }

  ret_val->first_read = 0;
  ret_val->is_last = 0;
  ret_val->header_nextP = 1;
  ret_val->fp = fp;
  ret_val->curr_header = (LimeRecordHeader *)NULL;
  ret_val->bytes_left = 0;
  ret_val->bytes_total = 0;
  ret_val->bytes_pad = 0;
  ret_val->eorP = 0;
  return ret_val;
}

void limeDestroyReader(LimeReader *r)
{
  if (r != (LimeReader *)NULL) { 

    if ( r->curr_header != (LimeRecordHeader *)NULL ) { 
      limeDestroyHeader(r->curr_header);
      r->curr_header = (LimeRecordHeader *)NULL;
    }
    free(r);

  }
}

int limeReaderNextRecord(LimeReader *r)
{
  int status;
  char myname[] = "limeReaderNextRecord";

  if( r == (LimeReader *)NULL ) { 
    printf("%s LIME_ERR_PARAM\n",myname);
    return LIME_ERR_PARAM;
  }
  
  if( r->first_read == 0 ) { 
    /* We haven't yet read the first record */
    /* Call an auxiliary function to read and parse the header 
       -- this call may allocate memory */
    status = readAndParseHeader(r);

    if ( status < 0 ) { 
      if( status != LIME_EOF )printf("%s returning %d\n",myname,status);
      return status;
    }

    /* We have started a new record */
    r->eorP = 0;

  }
  else { 
    /* We have read the first record -- so presumably we have 
       already got a header in the reader.*/

    /* In this case what we must do is skip to the end of the current
       record */
    status = skipReaderBytes(r, r->bytes_left);
    if ( status < 0 ) { 
      if( status != LIME_EOF )printf("%s returns %d\n",myname,status);
      return status;
    }
    
    /* We now allow multimessage files.  So the caller has to check the
       MB Flag.  After the end of a message, we expect a new message or EOF*/
    if( r->is_last ) { 
      r->first_read = 0;
    }

    /* Right we have now skipped to the end of the previous 
       record and we believe it not to be the last record 
       so we can now safely destroy the current record header 
       and try to read the next header */
    status = readAndParseHeader(r);
    if ( status < 0 ){
      if( status != LIME_EOF )printf("%s returns %d\n",myname,status);
      return status;
    }
    /* We have started a new record */
    r->eorP = 0;
  }


  /* If we were about to read the first record 
     then make note of the fact that we have just read it */
  if( r->first_read == 0 ) {
    r->first_read = 1;
  }

  r->is_last = r->curr_header->ME_flag;
  r->bytes_left = r->curr_header->data_length;
  r->bytes_total = r->curr_header->data_length;
  r->bytes_pad = lime_padding(r->bytes_total);

  return status;
}


int limeReaderReadData(void *dest, size_t *nbytes, LimeReader *r)
{
  int status;
  int bytes_to_read;
  int bytes_read;

  /* Check if we are at the end of the record */
  if( r->bytes_left == 0 ) {
    r->eorP = 1;
    return LIME_EOR;
  }

  /* If we are not at the end then read how much we have to */
  if( *nbytes > 0 ) { 
    if( *nbytes > r->bytes_left ) {
      bytes_to_read = r->bytes_left;
    }
    else { 
      bytes_to_read = *nbytes;
    }

    /* Actually read */
    bytes_read = fread(dest, sizeof(unsigned char), bytes_to_read, r->fp);
    *nbytes = bytes_read;
    
    if( bytes_read != bytes_to_read )
      return LIME_ERR_READ;

    r->bytes_left -= bytes_read;

    /* If as a result of this read we have reached the EOR then skip
       any padding */
    if( r->bytes_left == 0 ) { 
      status = skipReaderBytes(r, 0);
      if (status < 0 ) return status;
    }
  }

  return LIME_SUCCESS;
}

/* Advance to end of the current record */
/* This is good management when we have more than one node reading
   from the same file */
int limeReaderCloseRecord(LimeReader *r)
{
  /* printf("limeReaderCloseRecord not implemented\n"); */
  return 0;
}
      
/* Skip bytes within the current record.  If the skip takes us to the
   end of the data, skip any padding as well and set the end of record
   flag.  If the skip takes us past the end of the data in the current
   record, skip to end of the record and return an error. */

int skipReaderBytes(LimeReader *r, size_t bytes_to_skip)
{

  int status = LIME_SUCCESS;
  size_t bytes_to_seek;

  /* No backing up */
  if(bytes_to_skip < 0)return LIME_ERR_SEEK;

  /* Can't skip if we are at the end of the record already */
  if(r->eorP == 1){
    /* This is an error if the skip is non null */
    if(bytes_to_skip > 0){
      printf("Seeking %lu while already at end of record\n",
	     (unsigned long)bytes_to_skip);
      return LIME_ERR_SEEK;
    }
    /* If null skip and at end of record, ignore the request */
    else
      return status;
  }

  /* Prevent skip past the end of the data */
  if(r->bytes_left < bytes_to_skip){
    bytes_to_skip = r->bytes_left;
    printf("Seeking past end of data\n");
    status = LIME_ERR_SEEK;
  }

  /* If there will be no bytes left, include padding in the seek */
  bytes_to_seek = bytes_to_skip;
  if(r->bytes_left == bytes_to_skip)
    bytes_to_seek += r->bytes_pad;
    
  /* Seek */
  if(bytes_to_seek > 0){
    status = fseeko(r->fp, (off_t)bytes_to_seek, SEEK_CUR);
    if(status < 0){
      return LIME_ERR_SEEK;
      printf("fseek returned %d\n",status);
    }
  }

  /* Update the reader state */
  r->bytes_left -= bytes_to_skip;

  if(r->bytes_left == 0)
      r->eorP = 1;
  
  return status;

}

int limeReaderSeek(LimeReader *r, off_t offset, int whence){
  fprintf(stderr, "limeReaderSeek not implemented yet\n");
  return LIME_ERR_READ;
}


/* Accessors for header information */

/* Return MB flag in current header */
int limeReaderMBFlag(LimeReader *r){
  if(r == NULL)return -1;
  return r->curr_header->MB_flag;
}

/* Return ME flag in current header */
int limeReaderMEFlag(LimeReader *r){
  if(r == NULL)return -1;
  return r->curr_header->ME_flag;
}

/* Return pointer to LIME type string in current header */
char *limeReaderType(LimeReader *r){
  if(r == NULL)return NULL;
  return r->curr_header->type;
}

/* Return number of bytes in current record */
size_t limeReaderBytes(LimeReader *r){
  if(r == NULL)return 0;
  return r->bytes_total;
}


/* Entrance assumption to this function is that:
   i) The stream pointer is pointing to the beginning of 
      a record header.

   ii) There is a header about to come 
*/
int readAndParseHeader(LimeReader *r)
{

  unsigned int i_version;
  int i_MB, i_ME;
  n_uint32_t i_magic_no;
  size_t  i_data_length;
  unsigned char *typebuf;
  int status;
  char myname[] = "lime::readAndParseHeader";

  /* Destroy any old header structure kicking about */
  if( r->curr_header != (LimeRecordHeader *)NULL ) { 
    limeDestroyHeader(r->curr_header);
    r->curr_header = (LimeRecordHeader *)NULL;
  }

  /* Read the entire header */

  status = fread((void *)lime_header.int64, 
		 sizeof(n_uint64_t), MAX_HDR64, r->fp);
  if( status != MAX_HDR64 ) {
    if( feof( r->fp ) ) return LIME_EOF;
    fprintf(stderr,"%s read %d but wanted %d\n",myname,status,MAX_HDR64);
    return LIME_ERR_READ;
  }

  /* Check magic number */

  i_magic_no = big_endian_long(*lime_hdr_magic_no);
  if(i_magic_no != LIME_MAGIC_NO){
    fprintf(stderr,"%s: wrong magic number: read %x but wanted %x\n",
	    myname,i_magic_no,LIME_MAGIC_NO);
    return LIME_ERR_READ;
  }

#ifdef DEBUG
  fprintf(stderr, "%s Magic number OK: %d\n ", myname, i_magic_no);
  fflush(stderr);
#endif

  /* LIME version number */

  i_version = big_endian_short(*lime_hdr_version);

#ifdef DEBUG
  fprintf(stderr, "%s Input Version: %d\n ", myname,(int)i_version);
  fflush(stderr);
#endif

  /* MB flag */

  if ( *lime_hdr_mbme & MB_MASK ) { 
    i_MB = 1; 
  } 
  else {
    i_MB = 0;
  }

#ifdef DEBUG
  fprintf(stderr, "%s MB Flag: %d\n ", myname, (int)i_MB);
  fflush(stderr);
#endif
  
  /* ME Flag */

  if( *lime_hdr_mbme & ME_MASK ) {
    i_ME = 1;
  }
  else { 
    i_ME = 0;
  }

#ifdef DEBUG
  fprintf(stderr, "%s ME Flag: %d\n ", myname, (int)i_ME);
  fflush(stderr);
#endif

  /* Data length */

  i_data_length = big_endian_long_long(*lime_hdr_data_len);

#ifdef DEBUG
  fprintf(stderr, "%s Data Length: %d\n ", myname, (int)i_data_length);
  fflush(stderr);  
#endif

  /* Record type. */
  typebuf = (unsigned char *)lime_hdr_rec_type;

  /* Sanity Checking */
  /* Check Version */
  if( i_version != LIME_VERSION ) { 
    fprintf(stderr, "%s Unknown Lime Version\n",myname);
    exit(EXIT_FAILURE);
  }

#ifdef DEBUG
  printf("%s: type %s\n",myname,typebuf);
#endif

  /* If we are the first packet we MUST have MB flag set */
  if( (r->first_read == 0 && i_MB == 0) || 
      (r->first_read == 1 && i_MB == 1 )) { 
    fprintf(stderr, "%s MB Flag incorrect: first_read = %d MB=%d \n",
	    myname, r->first_read, i_MB);
    exit(EXIT_FAILURE);
  }

  r->curr_header = limeCreateHeader(i_MB, i_ME,
				    (char*)typebuf,
				    i_data_length);


  if (r->curr_header == (LimeRecordHeader *)NULL ) { 
    fprintf(stderr, "%s ERROR; Couldn't create header\n",myname);
    exit(EXIT_FAILURE);
  }

  return LIME_SUCCESS;
}


  
  
