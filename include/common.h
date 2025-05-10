#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stddef.h>

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

// Request structure
typedef struct{
    RequestType type;
    char username[MAX_USERNAME_LEN];
    char message[MAX_MSG_SIZE]; 
} Request;

// Response structure
typedef struct{
    ResponseType type;
    ResponseStatus status;
    char message[MAX_MSG_SIZE + MAX_USERNAME_LEN + 1];
} Response;

// Functions to map strings to the corresponding enums 
RequestType parse_request_type(const char *type_str);
ResponseType parse_response_type(const char *type_str);
ResponseStatus parse_response_status(const char *status_str);

// Functions to map the enums to corresponding strings
const char *request_type_to_string(RequestType type);
const char *response_type_to_string(ResponseType type);
const char *response_status_to_string(ResponseStatus status);

// Function to read the first 4 bytes of the request and interpret it as the request length
uint32_t read_request_len(const uint8_t *buffer);

// Function to write the request length into a request buffer
void write_request_len(uint8_t *buffer, uint32_t len);

// Functions to receive and send entire messages using TCP sockets
int recv_all(int sock_fd, void *buffer, size_t length);
int send_all(int sock_fd, const void *buffer, size_t length);

// Functions to encode and decode the request messages
int encode_request(const RequestType *type, uint8_t **buffer, uint32_t *out_len);
int decode_request(const uint8_t *data, uint32_t length, RequestType *type);

// Functions to encode and decode the response messages
int encode_response(const ResponseType *type, const ResponseStatus *status, uint8_t **buffer);
int decode_response(const uint8_t *data, ResponseStatus *status, ResponseType *type);

#endif