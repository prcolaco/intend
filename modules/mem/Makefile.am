INCLUDES = -I$(top_srcdir)/libintend
METASOURCES = AUTO
lib_LTLIBRARIES = libintend_mem.la
libintend_mem_la_LDFLAGS = -module -no-undefined -version-info 1:0:0
libintend_mem_la_LIBADD = $(top_builddir)/libintend/libintend.la -ldl
libintend_mem_la_SOURCES = intend_mem.c
