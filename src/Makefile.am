AM_CPPFLAGS = -Iinclude
nobase_include_HEADERS = \
										../include/libdoc.h
										../include/libdoc/doc.h \
										../include/libdoc/prl.h \
										../include/libdoc/sprm.h \
										../include/libdoc/paragraph_boundaries.h \
										../include/libdoc/cell_boundaries.h \
										../include/libdoc/direct_paragraph_formatting.h \
										../include/libdoc/direct_character_formatting.h \
										../include/libdoc/apply_properties.h \
										../include/libdoc/style_properties.h \
										../include/libdoc/retrieving_text.h

bin_PROGRAMS = doc2txt

lib_LTLIBRARIES = libdoc.la

doc2txt_SOURCES = doc2txt.c
doc2txt_CFLAGS = -g 
doc2txt_LDADD = libdoc.la

libdoc_la_SOURCES = cell_boundaries.c \
										direct_paragraph_formatting.c \
										direct_character_formatting.c \
									  doc.c \
									  prl.c \
										doc_parse.c \
										paragraph_boundaries.c \
										apply_properties.c \
										style_properties.c \
										retrieving_text.c
libdoc_la_LIBADD =
