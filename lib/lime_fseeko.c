#include <lime_config.h>
#include <stdio.h>

#ifndef HAVE_FSEEKO

#include "lime_fseeko.h"

int fseeko(FILE *stream, off_t offset, int whence) {
  return fseek(stream, (long)offset, whence);
}
 
off_t ftello(FILE *stream) {
  return (off_t)ftell(stream);
}

#endif

