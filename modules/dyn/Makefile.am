INCLUDES = -I$(top_srcdir)/libintend
METASOURCES = AUTO
lib_LTLIBRARIES = libintend_dyn.la
libintend_dyn_la_LDFLAGS = -module -no-undefined -version-info 1:0:0
libintend_dyn_la_LIBADD = $(top_builddir)/libintend/libintend.la -ldl
libintend_dyn_la_SOURCES = intend_dyn.c
