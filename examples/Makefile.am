
INCFLAGS=-I@top_srcdir@/include

AM_CFLAGS= $(INCFLAGS)


AM_LDFLAGS=-L@top_builddir@/lib 

LDADD = @LIBS@ -llime


.PHONY: build_lib
build_lib:
	cd @top_builddir@/lib ; $(MAKE)
