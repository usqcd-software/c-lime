
INCFLAGS=-I@top_srcdir@/include

AM_CFLAGS= $(INCFLAGS)


AM_LDFLAGS=-L@top_builddir@/lib 

LDADD = @LIBS@ -llime

check_PROGRAMS=lime_writer_test1 lime_reader_test1

lime_writer_test1_SOURCES = lime_writer_test1.c
lime_writer_test1_DEPENDENCIES = build_lib

lime_reader_test1_SOURCES = lime_reader_test1.c
lime_reader_test1_DEPENDENCIES = build_lib


.PHONY: build_lib
build_lib:
	cd @top_builddir@/lib ; $(MAKE)
