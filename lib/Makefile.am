
INCFLAGS=-I@top_srcdir@/include

AM_CFLAGS = $(INCFLAGS)
VPATH = @top_srcdir@/lib
VPATH += @top_srcdir@/include

HDRS=lime.h lime_defs.h lime_writer.h lime_header.h lime_utils.h \
	lime_reader.h lime_fseeko.h


lib_LIBRARIES = liblime.a

liblime_a_SOURCES = lime_writer.c  lime_header.c  lime_utils.c \
	lime_reader.c lime_fseeko.c
