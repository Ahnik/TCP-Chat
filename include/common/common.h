#ifndef COMMON_H
#define COMMON_H

// Constants
#define MAX_USERNAME_SIZE 13
#define MAX_PAYLOAD_SIZE 2048
#define MAX_MESSAGE_SIZE (MAX_PAYLOAD_SIZE - MAX_USERNAME_SIZE - 6)
#define HEADER_SIZE 4
#define MAX_CLIENTS 10

// Error codes
#define ERR_OK                    0
#define ERR_MEMORY               -1
#define ERR_INVALID              -2
#define ERR_NOT_FOUND            -3
#define ERR_BUFFER_SIZE_EXCEEDED -4
#define ERR_RECEIVING            -5
#define ERR_CONNECTION_CLOSED    -6

// Server port
#define SERVER_PORT 4221

// Server backlog
#define SERVER_BACKLOG 10

// Client prompts
#define QUIT_PROMPT "/quit"

// Function for handling errors
// It displays the error encountered and then kills the process
void exit_with_error(const char *fmt, ...);

// Function to convert an error code to a string
const char *error_to_string(int error);

#endif