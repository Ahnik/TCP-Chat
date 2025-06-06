#include "message.h"
#include "protocol.h"
#include "common.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

int encode_request(uint8_t **buffer, size_t buffer_capacity, uint32_t length, const Request *request){
    uint32_t net_length = htonl(length);
    const char *type_str = request_type_to_string(request->type);

    // Add the four bytes representing the length of the payload in network byte order to the buffer
    if(buffer_capacity <= sizeof(net_length)) return ERR_BUFFER_SIZE_EXCEEDED;
    uint8_t *pointer = memcpy(*buffer, &net_length, sizeof(net_length));
    pointer += sizeof(net_length);
    buffer_capacity -= sizeof(net_length);

    // Add the type of request to the buffer
    if(buffer_capacity <= strlen(type_str)) return ERR_BUFFER_SIZE_EXCEEDED;
    pointer = memcpy(pointer, type_str, strlen(type_str));
    pointer += strlen(type_str);
    buffer_capacity -= strlen(type_str);

    // Add the delimiter to the buffer
    if(buffer_capacity <= 1) return ERR_BUFFER_SIZE_EXCEEDED;
    pointer = memcpy(pointer, DELIMITER, DELIMITER_LENGTH);
    pointer++;
    buffer_capacity--;

    // Add the username of the client making the request to the buffer
    if(buffer_capacity <= strlen(request->username)) return ERR_BUFFER_SIZE_EXCEEDED;
    pointer = memcpy(pointer, request->username, strlen(request->username));
    pointer += strlen(request->username);
    buffer_capacity -= strlen(request->username);

    // For MSG or NAME requests, add another delimiter and the message to the buffer
    if((request->type == REQUEST_MSG) || (request->type == REQUEST_NAME)){
        if(buffer_capacity <= 1) return ERR_BUFFER_SIZE_EXCEEDED;
        pointer = memcpy(pointer, DELIMITER, DELIMITER_LENGTH);
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

    // Tokenize the payload separated by the delimiter
    char *payload_token = strtok(payload_duplicate, DELIMITER);
    if(!payload_token){
        free(payload_duplicate);
        return ERR_INVALID;
    }

    // The first token is the request type
    request->type = parse_request_type(payload_token);
    if(request->type == REQUEST_COUNT){
        free(payload_duplicate);
        return ERR_INVALID;
    }

    payload_token = strtok(NULL, DELIMITER);
    if(!payload_token){
        free(payload_duplicate);
        return ERR_INVALID;
    }
    if(strlen(payload_token) > MAX_USERNAME_LEN){
        free(payload_duplicate);
        return ERR_BUFFER_SIZE_EXCEEDED;
    }

    // The second token is the username of the client sending the request
    snprintf(request->username, MAX_USERNAME_LEN, "%s", payload_token);

    // In case of MSG and NAME requests, the third token will be the message
    if((request->type == REQUEST_MSG) || (request->type == REQUEST_NAME)){
        payload_token = strtok(NULL, DELIMITER);
        if(!payload_token){
            free(payload_duplicate);
            return ERR_INVALID;
        }
        if(strlen(payload_token) > MAX_MESSAGE_SIZE){
            free(payload_duplicate);
            return ERR_BUFFER_SIZE_EXCEEDED;
        }
        snprintf(request->message, MAX_MESSAGE_SIZE, "%s", payload_token);
    }

    free(payload_duplicate);
    return ERR_OK;
}

int encode_response(uint8_t **buffer, size_t buffer_capacity, uint32_t length, const Response *response){
    uint32_t net_length = htonl(length);
    const char *type_str = response_type_to_string(response->type);
    const char *status_str = response_status_to_string(response->status);

    // Add the four bytes representing the length of the payload in the network byte order to the buffer
    if(buffer_capacity <= sizeof(net_length)) return ERR_BUFFER_SIZE_EXCEEDED;
    uint8_t *pointer = memcpy(*buffer, &net_length, sizeof(net_length));
    pointer += sizeof(net_length);
    buffer_capacity -= sizeof(net_length);

    // Add the response type to the buffer
    if(buffer_capacity <= strlen(type_str)) return ERR_BUFFER_SIZE_EXCEEDED;
    pointer = memcpy(pointer, type_str, strlen(type_str));
    pointer += strlen(type_str);
    buffer_capacity -= strlen(type_str);

    // Add the delimiter to the buffer
    if(buffer_capacity <= 1) return ERR_BUFFER_SIZE_EXCEEDED;
    pointer = memcpy(pointer, DELIMITER, DELIMITER_LENGTH);
    pointer++;
    buffer_capacity--;

    // Add the response status type to the buffer
    if(buffer_capacity <= strlen(status_str)) return ERR_BUFFER_SIZE_EXCEEDED;
    pointer = memcpy(pointer, status_str, strlen(status_str));
    pointer += strlen(status_str);
    buffer_capacity -= strlen(status_str);

    // Add the message to the buffer if the response type is INFO
    if(response->type == RESPONSE_INFO){
        if(buffer_capacity <= 1) return ERR_BUFFER_SIZE_EXCEEDED;
        pointer = memcpy(pointer, DELIMITER, DELIMITER_LENGTH);
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

    // Tokenize the payload string using the delimiter
    char *payload_token = strtok(payload_duplicate, DELIMITER);
    if(!payload_token){
        free(payload_duplicate);
        return ERR_INVALID;
    }

    // The first token is the response type 
    response->type = parse_response_type(payload_token);
    if(response->type == RESPONSE_COUNT){
        free(payload_duplicate);
        return ERR_INVALID;
    }

    payload_token = strtok(NULL, DELIMITER);
    if(!payload_token){
        free(payload_duplicate);
        return ERR_INVALID;
    }

    // The second token is the response status
    response->status = parse_response_status(payload_token);
    if(response->status == STATUS_COUNT){
        free(payload_duplicate);
        return ERR_INVALID;
    }

    // If the response type is INFO, the third token is the message
    if(response->type == RESPONSE_INFO){
        payload_token = strtok(NULL, DELIMITER);
        if(!payload_token){
            free(payload_duplicate);
            return ERR_INVALID;
        }
        if(strlen(payload_token) > MAX_MESSAGE_SIZE){
            free(payload_duplicate);
            return ERR_INVALID;
        }
        snprintf(response->message, MAX_MESSAGE_SIZE, "%s", payload_token);
    }

    free(payload_duplicate);
    return ERR_OK;
}