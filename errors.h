#ifndef ERRORS_H
#define ERRORS_H

#include <stdio.h>
#include <stdlib.h>

#define err_exit((lua_State *L, const char *fmt, ...) \
	do {\
		va_list argp;\
		va_start(argp, fmt);\
		vfprintf(stderr, fmt, argp);\
		va_end(argp);\
		lua_close(L);\
		exit(EXIT_FAILURE);
	} while(0)

#endif //ERRORS_H