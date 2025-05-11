#include "common.h"
#include <stdarg.h>
#include <errno.h>
#include <stdio.h>

void exit_with_error(const char *fmt, ...){
    int errno_save = errno;
    va_list ap;

    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    fprintf(stdout, "\n");
    fflush(stdout);

    if(errno_save != 0){
        fprintf(stdout, "(errno = %d) : %s\n", errno_save, strerror(errno_save));
        fprintf(stdout, "\n");
        fflush(stdout);
    }
    va_end(ap);

    exit(1);
}