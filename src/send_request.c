#include "send_request.h"
#include "common.h"
#include "message.h"
#include "net.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

void send_request(int socketfd, RequestType type, const char *username, const char *message){
    // Allocate memory for the request
    Request *request = (Request *)malloc(sizeof(*request));
    if(request == NULL){
        fprintf(stderr, "Memory allocation for request failed!\n");
        fprintf(stderr, "(error = %d) : %s\n", errno, strerror(errno));
        fflush(stderr);
        return;
    }

    // Prepare the request
    request->type = type;
    snprintf(request->username, MAX_USERNAME_SIZE, "%s", username);
    snprintf(request->message, MAX_MESSAGE_SIZE, "%s", message);
    
    // Prepare the message buffer to be sent
    uint32_t length = request_length(request);
    uint8_t request_buffer[MAX_PAYLOAD_SIZE];
    memset(request_buffer, 0, MAX_PAYLOAD_SIZE);    
    uint8_t *ptr = request_buffer;      // Pointer to the first element of the buffer

    // Encode the request into the request buffer
    int error;
    if((error = encode_request(&ptr, MAX_PAYLOAD_SIZE, length, request)) != ERR_OK){
        fprintf(stderr, "Request encoding error!\n");
        fprintf(stderr, "%s", error_to_string(error));
        fprintf(stderr, "\n");
        fflush(stderr);
        free(request);
        return;
    }

    // Send the message 
    if((error = send_all(socketfd, request_buffer, length + HEADER_SIZE)) != ERR_OK){
        fprintf(stderr, "Request sending error!\n");
        fprintf(stderr, "%s", error_to_string(error));
        fprintf(stderr, "\n");
        fflush(stderr);
        free(request);
        return;
    }

    // Free up dynamically allocated memory
    free(request);
}