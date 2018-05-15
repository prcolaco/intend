SUBDIRS = libeval libmisc libparser libruntime libstdlib
lib_LTLIBRARIES = libintend.la
libintend_la_LDFLAGS = -module -export-dynamic -version-info 1:0:0
libintend_la_LIBADD = $(top_builddir)/libintend/libeval/libeval.la \
	$(top_builddir)/libintend/libmisc/libmisc.la $(top_builddir)/libintend/libparser/libparser.la \
	$(top_builddir)/libintend/libruntime/libruntime.la $(top_builddir)/libintend/libstdlib/libstdlib.la
include_HEADERS = intend.h
libintend_la_SOURCES = intend.c
