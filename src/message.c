#include "message.h"
#include "protocol.h"
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

int encode_request(uint8_t **buffer, size_t buffer_capacity, uint32_t length, const Request *request){
    uint32_t net_length = htonl(length);
    char *type_str = request_type_to_string(request->type);

    if(buffer_capacity <= sizeof(net_length)) return -1;
    uint8_t *pointer = memcpy(*buffer, &net_length, sizeof(net_length));
    pointer += sizeof(net_length);
    buffer_capacity -= sizeof(net_length);

    if(buffer_capacity <= strlen(type_str)) return -1;
    pointer = memcpy(pointer, type_str, strlen(type_str));
    pointer += strlen(type_str);
    buffer_capacity -= strlen(type_str);

    if(buffer_capacity <= 1) return -1;
    pointer = memset(pointer, '|', 1);
    pointer++;
    buffer_capacity--;

    if(buffer_capacity <= strlen(request->username)) return -1;
    pointer = memcpy(pointer, request->username, strlen(request->username));
    pointer += strlen(request->username);
    buffer_capacity -= strlen(request->username);

    if(buffer_capacity <= 1) return -1;
    pointer = memset(pointer, '|', 1);
    pointer++;
    buffer_capacity--;

    if(buffer_capacity < strlen(request->message)) return -1;
    pointer = memcpy(pointer, request->message, strlen(request->message));
    return 0;
}

int decode_request(Request *request, const char *payload){
    char *payload_duplicate = strdup(payload);
    if(!payload_duplicate) return -1;

    char *payload_token = strtok(payload_duplicate, "|");
    if(!payload_token) return -2;
    request->type = parse_request_type(payload_token);

    payload_token = strtok(NULL, "|");
    if(!payload_token) return -2;
    if(strlen(payload_token) > MAX_USERNAME_LEN) return -3;
    strcpy(request->username, payload_token);

    if((request->type == REQUEST_MSG) || (request->type == REQUEST_NAME)){
        payload_token = strtok(NULL, "|");
        if(!payload_token) return -2;
        if(strlen(payload_token) > MAX_MESSAGE_SIZE) return -3;
        strcpy(request->message, payload_token);
        free(payload_duplicate);
    }

    return 0;
}

int encode_response(uint8_t **buffer, size_t buffer_capacity, uint32_t length, const Response *response){
    uint32_t net_length = htonl(length);
    char *type_str = response_type_to_string(response->type);
    char *status_str = response_status_to_string(response->status);

    if(buffer_capacity <= sizeof(net_length)) return -1;
    uint8_t *pointer = memcpy(*buffer, &net_length, sizeof(net_length));
    pointer += sizeof(net_length);
    buffer_capacity -= sizeof(net_length);

    if(buffer_capacity <= strlen(type_str)) return -1;
    pointer = memcpy(pointer, type_str, strlen(type_str));
    pointer += strlen(type_str);
    buffer_capacity -= strlen(type_str);

    if(buffer_capacity <= 1) return -1;
    pointer = memset(pointer, '|', 1);
    pointer++;
    buffer_capacity--;

    if(buffer_capacity <= strlen(status_str)) return -1;
    pointer = memcpy(pointer, status_str, strlen(status_str));
    pointer += strlen(status_str);
    buffer_capacity -= strlen(status_str);

    if(buffer_capacity <= 1) return -1;
    pointer = memset(pointer, '|', 1);
    pointer++;
    buffer_capacity--;

    if(buffer_capacity < strlen(response->message)) return -1;
    pointer = memcpy(pointer, response->message, strlen(response->message));
    return 0;
}

int decode_response(Response *response, const char *payload){
    char *payload_duplicate = strdup(payload);
    if(!payload_duplicate) return -1;

    char *payload_token = strtok(payload_duplicate, "|");
    if(!payload_token) return -2;
    response->type = parse_response_type(payload_token);

    payload_token = strtok(NULL, "|");
    if(!payload_token) return -2;
    response->status = parse_response_status(payload_token);

    if(response->type == RESPONSE_INFO){
        payload_token = strtok(NULL, "|");
        if(!payload_token) return -2;
        if(strlen(payload_token) > MAX_MESSAGE_SIZE) return -3;
        strcpy(response->message, payload_token);
    }

    return 0;
}