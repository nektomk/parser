#ifndef DEBUG_H
#define DEBUG_H 1

#include <stdio.h>

#define TRACE(fmt,...) do { \
	fprintf(stderr,"%s:%d %s ",__FILE__,__LINE__,__PRETTY_FUNCTION__); \
	if (fmt && fmt[0]) fprintf(stderr,fmt,##__VA_ARGS__); \
	else fprintf(stderr,"TRACE"); \
	fputc('\n',stderr); \
} while(0)

#endif // DEBUG_H
