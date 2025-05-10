#ifndef MESSAGE_H
#define MESSAGE_H

#include "common.h"
#include <stdint.h>

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

// Functions to encode and decode the request messages
int encode_request(const RequestType *type, uint8_t **buffer, uint32_t *out_len);
int decode_request(const uint8_t *data, uint32_t length, RequestType *type);

// Functions to encode and decode the response messages
int encode_response(const ResponseType *type, const ResponseStatus *status, uint8_t **buffer);
int decode_response(const uint8_t *data, ResponseStatus *status, ResponseType *type);

#endif