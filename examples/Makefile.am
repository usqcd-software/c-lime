
INCFLAGS=-I@top_srcdir@/include

AM_CFLAGS= $(INCFLAGS)


AM_LDFLAGS=-L@top_builddir@/lib 

LDADD = @LIBS@ -llime

#
# The programs to build
#
check_PROGRAMS = lime_contents lime_writer_test1 lime_extract_record lime_pack lime_unpack

#
# The program and its dependencies
#
lime_contents_SOURCES = lime_contents.c
lime_writer_test1_SOURCES = lime_writer_test1.c
lime_extract_record_SOURCES = lime_extract_record.c
lime_pack_SOURCES = lime_pack.c
lime_unpack_SOURCES = lime_unpack.c


.PHONY: build_lib
build_lib:
	cd @top_builddir@/lib ; $(MAKE)
