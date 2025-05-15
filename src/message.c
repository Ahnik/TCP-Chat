#include "message.h"
#include "protocol.h"
#include "error.h"
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

int encode_request(uint8_t **buffer, size_t buffer_capacity, uint32_t length, const Request *request){
    uint32_t net_length = htonl(length);
    const char *type_str = request_type_to_string(request->type);

    if(buffer_capacity <= sizeof(net_length)) return ERR_BUFFER_SIZE_EXCEEDED;
    uint8_t *pointer = memcpy(*buffer, &net_length, sizeof(net_length));
    pointer += sizeof(net_length);
    buffer_capacity -= sizeof(net_length);

    if(buffer_capacity <= strlen(type_str)) return ERR_BUFFER_SIZE_EXCEEDED;
    pointer = memcpy(pointer, type_str, strlen(type_str));
    pointer += strlen(type_str);
    buffer_capacity -= strlen(type_str);

    if(buffer_capacity <= 1) return ERR_BUFFER_SIZE_EXCEEDED;
    pointer = memset(pointer, '|', 1);
    pointer++;
    buffer_capacity--;

    if(buffer_capacity <= strlen(request->username)) return ERR_BUFFER_SIZE_EXCEEDED;
    pointer = memcpy(pointer, request->username, strlen(request->username));
    pointer += strlen(request->username);
    buffer_capacity -= strlen(request->username);

    if((request->type == REQUEST_MSG) || (request->type == REQUEST_NAME)){
        if(buffer_capacity <= 1) return ERR_BUFFER_SIZE_EXCEEDED;
        pointer = memset(pointer, '|', 1);
        pointer++;
        buffer_capacity--;

        if(buffer_capacity < strlen(request->message)) return ERR_BUFFER_SIZE_EXCEEDED;
        pointer = memcpy(pointer, request->message, strlen(request->message));
    }
    return ERR_OK;
}

int decode_request(Request *request, const char *payload){
    char *payload_duplicate = strdup(payload);
    if(!payload_duplicate) return ERR_MEMORY;

    char *payload_token = strtok(payload_duplicate, "|");
    if(!payload_token){
        free(payload_duplicate);
        return ERR_INVALID;
    }
    request->type = parse_request_type(payload_token);
    if(request->type == REQUEST_COUNT){
        free(payload_duplicate);
        return ERR_INVALID;
    }

    payload_token = strtok(NULL, "|");
    if(!payload_token){
        free(payload_duplicate);
        return ERR_INVALID;
    }
    if(strlen(payload_token) > MAX_USERNAME_LEN){
        free(payload_duplicate);
        return ERR_BUFFER_SIZE_EXCEEDED;
    }
    strcpy(request->username, payload_token);

    if((request->type == REQUEST_MSG) || (request->type == REQUEST_NAME)){
        payload_token = strtok(NULL, "|");
        if(!payload_token){
            free(payload_duplicate);
            return ERR_INVALID;
        }
        if(strlen(payload_token) > MAX_MESSAGE_SIZE){
            free(payload_duplicate);
            return ERR_BUFFER_SIZE_EXCEEDED;
        }
        strcpy(request->message, payload_token);
    }

    free(payload_duplicate);
    return ERR_OK;
}

int encode_response(uint8_t **buffer, size_t buffer_capacity, uint32_t length, const Response *response){
    uint32_t net_length = htonl(length);
    const char *type_str = response_type_to_string(response->type);
    const char *status_str = response_status_to_string(response->status);

    if(buffer_capacity <= sizeof(net_length)) return ERR_BUFFER_SIZE_EXCEEDED;
    uint8_t *pointer = memcpy(*buffer, &net_length, sizeof(net_length));
    pointer += sizeof(net_length);
    buffer_capacity -= sizeof(net_length);

    if(buffer_capacity <= strlen(type_str)) return ERR_BUFFER_SIZE_EXCEEDED;
    pointer = memcpy(pointer, type_str, strlen(type_str));
    pointer += strlen(type_str);
    buffer_capacity -= strlen(type_str);

    if(buffer_capacity <= 1) return ERR_BUFFER_SIZE_EXCEEDED;
    pointer = memset(pointer, '|', 1);
    pointer++;
    buffer_capacity--;

    if(buffer_capacity <= strlen(status_str)) return ERR_BUFFER_SIZE_EXCEEDED;
    pointer = memcpy(pointer, status_str, strlen(status_str));
    pointer += strlen(status_str);
    buffer_capacity -= strlen(status_str);

    if(response->type == RESPONSE_INFO){
        if(buffer_capacity <= 1) return ERR_BUFFER_SIZE_EXCEEDED;
        pointer = memset(pointer, '|', 1);
        pointer++;
        buffer_capacity--;

        if(buffer_capacity < strlen(response->message)) return ERR_BUFFER_SIZE_EXCEEDED;
        pointer = memcpy(pointer, response->message, strlen(response->message));
    }
    return ERR_OK;
}

int decode_response(Response *response, const char *payload){
    char *payload_duplicate = strdup(payload);
    if(!payload_duplicate) return ERR_MEMORY;

    char *payload_token = strtok(payload_duplicate, "|");
    if(!payload_token){
        free(payload_duplicate);
        return ERR_INVALID;
    }
    response->type = parse_response_type(payload_token);
    if(response->type == RESPONSE_COUNT){
        free(payload_duplicate);
        return ERR_INVALID;
    }

    payload_token = strtok(NULL, "|");
    if(!payload_token){
        free(payload_duplicate);
        return ERR_INVALID;
    }
    response->status = parse_response_status(payload_token);
    if(response->status == STATUS_COUNT){
        free(payload_duplicate);
        return ERR_INVALID;
    }

    if(response->type == RESPONSE_INFO){
        payload_token = strtok(NULL, "|");
        if(!payload_token){
            free(payload_duplicate);
            return ERR_INVALID;
        }
        if(strlen(payload_token) > MAX_MESSAGE_SIZE){
            free(payload_duplicate);
            return ERR_INVALID;
        }
        strcpy(response->message, payload_token);
    }

    free(payload_duplicate);
    return ERR_OK;
}