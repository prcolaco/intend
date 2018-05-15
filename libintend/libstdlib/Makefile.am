METASOURCES = AUTO
DEFAULT_INCLUDES = -I$(top_builddir)
noinst_LTLIBRARIES = libstdlib.la
libstdlib_la_SOURCES = array.c env.c function.c locale.c modules.c random.c \
	runtime.c stdlib.c string.c struct.c time.c
noinst_HEADERS = array.h env.h function.h locale.h modules.h random.h runtime.h \
	stdlib.h string.h struct.h time.h
libstdlib_la_LDFLAGS = -avoid-version
