
INCFLAGS=-I@top_srcdir@/include

AM_CFLAGS= $(INCFLAGS)


AM_LDFLAGS=-L@top_builddir@/lib 

LDADD = @LIBS@ -llime

#
# The programs to build
#
check_PROGRAMS = lime_reader_test1 lime_writer_test1

#
# The program and its dependencies
#
lime_reader_test1_SOURCES = lime_reader_test1.c
lime_writer_test1_SOURCES = lime_writer_test1.c


.PHONY: build_lib
build_lib:
	cd @top_builddir@/lib ; $(MAKE)
