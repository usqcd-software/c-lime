#ifndef LIME_WRITER_H
#define LIME_WRITER_H

#include <lime_defs.h>
#include <lime_header.h>
#include <sys/types.h>
#include <stdio.h>

/** \brief This describes the state of LIME */
typedef struct { 
  int first_record;        /**< Are we to write the first record */
  int last_written;        /**< has the last record been written */
  FILE* fp;                /**< What file are we writing to */
  int header_nextP;        /**< Are we to write a header next or data */
  off_t bytes_total;      /**< Total no of bytes in this record */
  off_t bytes_left;       /**< The number of bytes left in the current
                                 record */
  size_t bytes_pad;        /**< The number of bytes to pad the record */
  int isLastP;             /**< Is this the last record in the message? */
} LimeWriter;

/** \brief Initialise the lime state. Currently with an open file
    -- this may change
    \param fp the FILE to which we want to write 
    \returns a LimeWriter* which you need for further
    transactions
*/
LimeWriter* limeCreateWriter(FILE *fp);

/** \brief Close a lime generator */
int limeDestroyWriter(LimeWriter *s);

/** Write a lime record header
 *
 *\param d is the handle of the LIME stream
 *\param props contains data for the record header 
 *             -- note we fill in some fields depending on 
 *                circumstances
 *\param lastP  is a boolean indicating whether this is to be the 
 *              last record
 */
int limeWriteRecordHeader( LimeRecordHeader *props, LimeWriter* d);

/** Write a lime record
 *
 *\param source is the buffer or writing
 *\param nbytes number of bytes to write. Is modified with number 
 *              of bytes actually written
 *\param d  is the LimeWriter/generator to use
 */
int limeWriteRecordData( void *source, off_t *nbytes,  LimeWriter* d);

/** \brief Advance to end of current record
 *  \params w points to a LimeWriter
 */

int limeWriterCloseRecord(LimeWriter *w);

/** \brief Seek bytes within current open record 
 *  \params r is a pointer to a LimeWriter
 *  \params offset counts bytes from the current position ("WHENCE")
 *
 *  \returns a status code
 */
int limeWriterSeek(LimeWriter *r, off_t offset, int whence);

#endif
