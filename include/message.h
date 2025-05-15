#ifndef MESSAGE_H
#define MESSAGE_H

#include "common.h"
#include "protocol.h"
#include <stddef.h>
#include <stdint.h>

// Request structure
typedef struct{
    RequestType type;
    char username[MAX_USERNAME_LEN];
    char message[MAX_MESSAGE_SIZE]; 
} Request;

// Response structure
typedef struct{
    ResponseType type;
    ResponseStatus status;
    char message[MAX_MESSAGE_SIZE];
} Response;

// Function encodes the request according to the protocol into the given buffer
int encode_request(uint8_t **buffer, size_t buffer_capacity, uint32_t length, const Request *request);

// Function decodes the request the payload to fill up the request struct
int decode_request(Request *request, const char *payload);

// Function encodes the response according to the protocol into the given buffer
int encode_response(uint8_t **buffer, size_t buffer_capacity, uint32_t length, const Response *response);

// Function decodes the response the payload to fill up the response struct
int decode_response(Response *response, const char *payload);

#endif