#include "send_response.h"
#include "common.h"
#include "message.h"
#include "net.h"
#include "clients.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

void send_response_to_client(int socketfd, ResponseType type, ResponseStatus status, const char *message){
    // Allocate memory for the response
    Response *response = (Response *)malloc(sizeof(*response));
    if(response == NULL){
        fprintf(stderr, "Memory allocation for response failed!\n");
        fprintf(stderr, "(error = %d) : %s\n", errno, strerror(errno));
        fflush(stderr);
        return;
    }

    // Prepare the response
    response->type = type;
    response->status = status;
    if(strlen(message) > 0)
        snprintf(response->message, MAX_MESSAGE_SIZE, "%s", message);

    // Prepare the message buffer to be sent to the client
    uint32_t length = response_length(response);
    uint8_t response_buffer[MAX_PAYLOAD_SIZE];
    memset(response_buffer, 0, MAX_PAYLOAD_SIZE);
    uint8_t *ptr = response_buffer;     // Pointer to the first element of the buffer

    // Encode the response into the response buffer
    int error;
    if((error = encode_response(&ptr, MAX_PAYLOAD_SIZE, length, response)) != ERR_OK){
        fprintf(stderr, "Response encoding error!\n");
        fprintf(stderr, "%s", error_to_string(error));
        fprintf(stderr, "\n");
        fflush(stderr);
        free(response);
        return;
    }

    // Send the message to the client
    if((error = send_all(socketfd, response_buffer, length + HEADER_SIZE)) != ERR_OK){
        fprintf(stderr, "Response sending error!\n");
        fprintf(stderr, "%s", error_to_string(error));
        fprintf(stderr, "\n");
        fflush(stderr);
        free(response);
        return;
    }

    // Free up dynamically allocated memory
    free(response);
}

void broadcast_response(ResponseType type, ResponseStatus status, const char *message){
    // Allocate memory for the response
    Response *response = (Response *)malloc(sizeof(*response));
    if(response == NULL){
        fprintf(stderr, "Memory allocation for response failed!\n");
        fprintf(stderr, "(error = %d) : %s\n", errno, strerror(errno));
        fflush(stderr);
        return;
    }

    // Prepare the response
    response->type = type;
    response->status = status;
    snprintf(response->message, MAX_MESSAGE_SIZE, "%s", message);

    // Prepare the message buffer to be sent to the client
    uint32_t length = response_length(response);
    uint8_t response_buffer[MAX_PAYLOAD_SIZE];
    memset(response_buffer, 0, MAX_PAYLOAD_SIZE);
    uint8_t *ptr = response_buffer;     // Pointer to the first element of the buffer

    // Encode the response into the response buffer
    int error;
    if((error = encode_response(&ptr, MAX_PAYLOAD_SIZE, length, response)) != ERR_OK){
        fprintf(stderr, "Response encoding error!\n");
        fprintf(stderr, "%s", error_to_string(error));
        fprintf(stderr, "\n");
        fflush(stderr);
        free(response);
        return;
    }

    // Broadcast messages to all clients
    if((error = broadcast_message(response_buffer, length + HEADER_SIZE)) != ERR_OK){
        fprintf(stderr, "Unable to send all messages\n");
        fprintf(stderr, "%s", error_to_string(error));
        fprintf(stderr, "\n");
        fflush(stderr);
        free(response);
        return;
    }

    // Free up dynamically allocated memory
    free(response);
}

void send_error_to_client(int client_socket, ResponseStatus status){
    send_response_to_client(client_socket, RESPONSE_ERR, status, "");
}