#ifndef COMMON_H
#define COMMON_H

// Constants
#define MAX_USERNAME_LEN 12
#define MAX_MSG_SIZE 1024
#define HEADER_SIZE 4

// Function for handling errors
// It displays the error encountered and then kills the process
void exit_with_error(const char *fmt, ...);

#endif