#ifndef ERRORS_H
#define ERRORS_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "lua.h"

void err_exit(lua_State *L, const char *fmt, ...) {
    va_list argp;
    va_start(argp, fmt);
    vfprintf(stderr, fmt, argp);
    va_end(argp);
    lua_close(L);
    exit(EXIT_FAILURE);
}

#endif //ERRORS_H
