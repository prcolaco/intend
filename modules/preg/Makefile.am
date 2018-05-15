INCLUDES = -I$(top_srcdir)/libintend
METASOURCES = AUTO
libintend_preg_la_LDFLAGS = -module -no-undefined -version-info 1:0:0
lib_LTLIBRARIES = libintend_preg.la
libintend_preg_la_LIBADD = $(top_builddir)/libintend/libintend.la -ldl -lpcre
libintend_preg_la_SOURCES = intend_preg.c
