INCLUDES = 
METASOURCES = AUTO
noinst_HEADERS = parser.h
noinst_LTLIBRARIES = libparser.la
libparser_la_SOURCES = expr_dump.c expr_memory.c expr_parse.c expr_stack.c \
	icl_grammar.y icl_lexer.l parser.c stmt_dump.c stmt_list.c stmt_memory.c stmt_parse.c \
	stmt_stack.c
BUILT_SOURCES = icl_grammar.h
AM_YFLAGS = -d
libparser_la_LDFLAGS = -avoid-version
