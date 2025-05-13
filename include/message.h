#ifndef MESSAGE_H
#define MESSAGE_H

#include "common.h"
#include "protocol.h"
#include <stdint.h>

// Request structure
typedef struct{
    RequestType type;
    char username[MAX_USERNAME_LEN];
    char message[MAX_PAYLOAD_SIZE]; 
} Request;

// Response structure
typedef struct{
    ResponseType type;
    ResponseStatus status;
    char message[MAX_PAYLOAD_SIZE];
} Response;

// Functions to encode and decode the request messages
int encode_request(uint8_t **buffer,size_t buffer_capacity, uint32_t length, const Request *request);
int decode_request(const uint8_t *data, uint32_t length, RequestType *type);

// Functions to encode and decode the response messages
int encode_response(const ResponseType *type, const ResponseStatus *status, uint8_t **buffer);
int decode_response(const uint8_t *data, ResponseStatus *status, ResponseType *type);

#endif