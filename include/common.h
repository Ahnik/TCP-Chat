#ifndef COMMON_H
#define COMMON_H

#include <errno.h>

// Constants
#define MAX_USERNAME_LEN 12
#define MAX_MSG_SIZE 1024
#define HEADER_SIZE 4

// The types of requests supported
typedef enum{
    REQUEST_MSG = 0,    
    REQUEST_JOIN, 
    REQUEST_LEAVE,
    REQUEST_NAME,
    REQUEST_COUNT
} RequestType;

// The types of responses supported
typedef enum{
    RESPONSE_ACK = 0,
    RESPONSE_ERR,
    RESPONSE_INFO,
    RESPONSE_COUNT
} ResponseType;

// The types of response statuses supported
typedef enum{
    // Status for ACK responses
    STATUS_ACK_OK = 0,
    STATUS_ACK_NAME_SET,
    STATUS_ACK_JOINED,
    STATUS_ACK_QUIT,

    // Status for ERR responses
    STATUS_ERR_INVALID_CMD,
    STATUS_ERR_MALFORMED,
    STATUS_ERR_NAME_TAKEN,
    STATUS_ERR_NAME_MISSING,
    STATUS_ERR_NOT_LOGGED_IN,

    // Status for INFO responses
    STATUS_INFO_USER_JOINED,
    STATUS_INFO_USER_LEFT,
    STATUS_INFO_MESSAGE,
    STATUS_INFO_SERVER_SHUTDOWN,
    STATUS_COUNT
} ResponseStatus;

// Function for handling errors
// It displays the error encountered and then kills the process
void exit_with_error(const char *fmt, ...);

#endif