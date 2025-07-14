#include "common.h"
#include <stdarg.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void exit_with_error(const char *fmt, ...){
    int errno_save = errno;
    va_list ap;

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    fflush(stderr);

    if(errno_save != 0){
        fprintf(stderr, "(errno = %d) : %s\n", errno_save, strerror(errno_save));
        fprintf(stderr, "\n");
        fflush(stderr);
    }
    va_end(ap);

    exit(1);
}

const char *error_to_string(int error){
    switch(error){
        case ERR_OK:                   return "No error";
        case ERR_MEMORY:               return "Memory allocation failed";
        case ERR_INVALID:              return "Bad arguments";
        case ERR_BUFFER_SIZE_EXCEEDED: return "Buffer size exceeded";
        case ERR_RECEIVING:            return "Receiving error";
        case ERR_CONNECTION_CLOSED:    return "Connection has been closed";
        default:                       return "Unknown error";
    }
}