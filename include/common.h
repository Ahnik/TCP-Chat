#ifndef COMMON_H
#define COMMON_H

// Constants
#define MAX_USERNAME_LEN 12
#define MAX_PAYLOAD_SIZE 2048
#define MAX_MESSAGE_SIZE (MAX_PAYLOAD_SIZE - MAX_USERNAME_LEN - 6)
#define HEADER_SIZE 4

// Error codes
#define ERR_OK 0
#define ERR_MEMORY -1
#define ERR_INVALID -2
#define ERR_BUFFER_SIZE_EXCEEDED -3
#define ERR_RECEIVING -4

// Function for handling errors
// It displays the error encountered and then kills the process
void exit_with_error(const char *fmt, ...);

// Function to convert an error code to a string
const char *error_to_string(int error);

#endif