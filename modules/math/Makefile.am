INCLUDES = -I$(top_srcdir)/libintend
METASOURCES = AUTO
lib_LTLIBRARIES = libintend_math.la
libintend_math_la_LDFLAGS = -module -no-undefined -version-info 1:0:0
libintend_math_la_LIBADD = $(top_builddir)/libintend/libintend.la -ldl -lm
noinst_HEADERS = intend_math.h
libintend_math_la_SOURCES = intend_math.c
