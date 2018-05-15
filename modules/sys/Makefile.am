INCLUDES = -I$(top_srcdir)/libintend
METASOURCES = AUTO
lib_LTLIBRARIES = libintend_sys.la
libintend_sys_la_LDFLAGS = -module -no-undefined -version-info 1:0:0
libintend_sys_la_LIBADD = $(top_builddir)/libintend/libintend.la -ldl
libintend_sys_la_SOURCES = intend_sys.c
