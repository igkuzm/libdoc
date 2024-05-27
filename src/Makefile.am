AM_CPPFLAGS = -Iinclude
nobase_include_HEADERS = \
										../include/libdoc.h
										../include/libdoc/doc.h \
										../include/libdoc/cell_boundaries.h \
										../include/libdoc/direct_paragraph_formatting.h \
										../include/libdoc/retrieving_text.h

bin_PROGRAMS = doc2txt
doc2txt_SOURCES = doc2txt.c
doc2txt_CFLAGS = 
doc2txt_LDADD = libdoc.la


lib_LTLIBRARIES = libdoc.la
libdoc_la_SOURCES = cell_boundaries.c \
										direct_paragraph_formatting.c \
										retrieving_text.c
libdoc_la_LIBADD =
