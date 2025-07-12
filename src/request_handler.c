#include "request_handler.h"
#include "clients.h"
#include "message.h"
#include "net.h"
#include "protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

void handle_name_request(int client_socket, const Request *request){
    // Find the client sending the request
    Client *client = find_client_by_socket(client_socket);
    if(client == NULL){
        fprintf(stderr, "Unable to find client!\n");
        fflush(stderr);
        send_error_to_client(client_socket, STATUS_ERR_NOT_LOGGED_IN);
        return;
    }

    // Allocate memory for the response
    Response *response = (Response *)malloc(sizeof(*response));
    if(response == NULL){
        fprintf(stderr, "Memory allocation for response failed!\n");
        fprintf(stderr, "(error = %d) : %s\n", errno, strerror(errno));
        fflush(stderr);
        return;
    }

    // Store the previous username of the client 
    char prev_name[MAX_USERNAME_SIZE];
    snprintf(prev_name, MAX_USERNAME_SIZE, "%s", client->username);

    // Change the name of the client 
    snprintf(client->username, MAX_USERNAME_SIZE, "%s", request->message);

    // Prepare the acknowledgement response for the client
    response->type = RESPONSE_ACK;
    response->status = STATUS_ACK_NAME_SET;

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
    if((error = send_all(client_socket, response_buffer, length + HEADER_SIZE)) != ERR_OK){
        fprintf(stderr, "Response sending error!\n");
        fprintf(stderr, "%s", error_to_string(error));
        fprintf(stderr, "\n");
        fflush(stderr);
        free(response);
        return;
    }

    // Prepare the information response  to be broadcasted to all clients
    response->type = RESPONSE_INFO;
    response->status = STATUS_INFO_NAME_CHANGED;
    snprintf(response->message, MAX_MESSAGE_SIZE, "--%s has changed their name to %s--", prev_name, client->username);

    // Clear the response buffer and update the length
    memset(response_buffer, 0, MAX_PAYLOAD_SIZE);
    ptr = response_buffer;
    length = response_length(response);

    // Encode the response into the response buffer
    if((error = encode_response(&ptr, MAX_PAYLOAD_SIZE, length, response)) != ERR_OK){
        fprintf(stderr, "Response encoding error!\n");
        fprintf(stderr, "%s", error_to_string(error));
        fprintf(stderr, "\n");
        fflush(stderr);
        free(response);
        return;
    }

    // Send the response to all the clients
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

void handle_msg_request(int client_socket, const Request *request){
    // Find the client sending the request
    Client *client = find_client_by_socket(client_socket);
    if(client == NULL){
        fprintf(stderr, "Unable to find client!\n");
        fflush(stderr);
        send_error_to_client(client_socket, STATUS_ERR_NOT_LOGGED_IN);
        return;
    }

    // Allocate memory for the response
    Response *response = (Response *)malloc(sizeof(*response));
    if(response == NULL){
        fprintf(stderr, "Memory allocation for response failed!\n");
        fprintf(stderr, "(error = %d) : %s\n", errno, strerror(errno));
        fflush(stderr);
        return;
    }

    // Prepare the acknowledgement response for the client
    response->type = RESPONSE_ACK;
    response->status = STATUS_ACK_OK;

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
    if((error = send_all(client_socket, response_buffer, length + HEADER_SIZE)) != ERR_OK){
        fprintf(stderr, "Response sending error!\n");
        fprintf(stderr, "%s", error_to_string(error));
        fprintf(stderr, "\n");
        fflush(stderr);
        free(response);
        return;
    }

    // Prepare the message to be sent to all the clients
    char response_message[MAX_MESSAGE_SIZE];
    snprintf(response_message, MAX_MESSAGE_SIZE, "%s: %s", request->username, request->message);

    // Prepare the information response to be broadcasted to all clients
    response->type = RESPONSE_INFO;
    response->status = STATUS_INFO_MESSAGE;
    snprintf(response->message, MAX_MESSAGE_SIZE, "%s", response_message);

    // Clear the response buffer and update the length
    memset(response_buffer, 0, MAX_PAYLOAD_SIZE);
    ptr = response_buffer;
    length = response_length(response);

    // Encode the response into the response buffer
    if((error = encode_response(&ptr, MAX_PAYLOAD_SIZE, length, response)) != ERR_OK){
        fprintf(stderr, "Response encoding error!\n");
        fprintf(stderr, "%s", error_to_string(error));
        fprintf(stderr, "\n");
        fflush(stderr);
        free(response);
        return;
    }

    // Send the response to all the clients
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

void handle_join_request(int client_socket, const Request *request){
    // Check if the there is already a client by that name
    if(find_client_by_name(request->username) != NULL){
        send_error_to_client(client_socket, STATUS_ERR_NAME_TAKEN);
        return;
    }

    // Allocate memory for the response
    Response *response = (Response *)malloc(sizeof(*response));
    if(response == NULL){
        fprintf(stderr, "Memory allocation for response failed!\n");
        fprintf(stderr, "(error = %d) : %s\n", errno, strerror(errno));
        fflush(stderr);
        return;
    }

    // Join the client
    int error;
    if((error = add_client(client_socket, request->username)) != ERR_OK){
        fprintf(stderr, "Unable to add client!\n");
        fprintf(stderr, "%s", error_to_string(error));
        fprintf(stderr, "\n");
        fflush(stderr);
        free(response);
        send_error_to_client(client_socket, STATUS_ERR_SERVER_ERROR);
        return;
    }

    // Prepare the acknowledgement response for the client
    response->type = RESPONSE_ACK;
    response->status = STATUS_ACK_JOINED;

    // Prepare the message buffer to be sent to the client
    uint32_t length = response_length(response);
    uint8_t response_buffer[MAX_PAYLOAD_SIZE];
    memset(response_buffer, 0, MAX_PAYLOAD_SIZE);
    uint8_t *ptr = response_buffer;     // Pointer to the first element of the buffer

    // Encode the response into the response buffer
    if((error = encode_response(&ptr, MAX_PAYLOAD_SIZE, length, response)) != ERR_OK){
        fprintf(stderr, "Response encoding error!\n");
        fprintf(stderr, "%s", error_to_string(error));
        fprintf(stderr, "\n");
        fflush(stderr);
        free(response);
        return;
    }

    // Send the message to the client
    if((error = send_all(client_socket, response_buffer, length + HEADER_SIZE)) != ERR_OK){
        fprintf(stderr, "Response sending error!\n");
        fprintf(stderr, "%s", error_to_string(error));
        fprintf(stderr, "\n");
        fflush(stderr);
        free(response);
        return;
    }

    // Prepare the message to be sent to all the clients
    char response_message[MAX_MESSAGE_SIZE];
    snprintf(response_message, MAX_MESSAGE_SIZE, "--%s has joined--", request->username);

    // Prepare the information response to be broadcasted to all clients
    response->type = RESPONSE_INFO;
    response->status = STATUS_INFO_USER_JOINED;
    snprintf(response->message, MAX_MESSAGE_SIZE, "%s", response_message);

    // Clear the response buffer and update the length
    memset(response_buffer, 0, MAX_PAYLOAD_SIZE);
    ptr = response_buffer;
    length = response_length(response);

    // Encode the response into the response buffer
    if((error = encode_response(&ptr, MAX_PAYLOAD_SIZE, length, response)) != ERR_OK){
        fprintf(stderr, "Response encoding error!\n");
        fprintf(stderr, "%s", error_to_string(error));
        fprintf(stderr, "\n");
        fflush(stderr);
        free(response);
        return;
    }

    // Send the response to all the clients
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

void handle_leave_request(int client_socket, const Request *request){
    // Find the client sending the request
    Client *client = find_client_by_socket(client_socket);
    if(client == NULL){
        fprintf(stderr, "Unable to find client!\n");
        fflush(stderr);
        send_error_to_client(client_socket, STATUS_ERR_NOT_LOGGED_IN);
        return;
    }

    // Allocate memory for the response
    Response *response = (Response *)malloc(sizeof(*response));
    if(response == NULL){
        fprintf(stderr, "Memory allocation for response failed!\n");
        fprintf(stderr, "(error = %d) : %s\n", errno, strerror(errno));
        fflush(stderr);
        return;
    }

    // Prepare the acknowledgement response
    response->type = RESPONSE_ACK;
    response->status = STATUS_ACK_QUIT;

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
    if((error = send_all(client_socket, response_buffer, length + HEADER_SIZE)) != ERR_OK){
        fprintf(stderr, "Response sending error!\n");
        fprintf(stderr, "%s", error_to_string(error));
        fprintf(stderr, "\n");
        fflush(stderr);
        free(response);
        return;
    }

    // Remove the client
    if(remove_client(client_socket) != ERR_OK){
        fprintf(stderr, "Unable to remove client!\n");
        fprintf(stderr, "%s", error_to_string(error));
        fprintf(stderr, "\n");
        fflush(stderr);
        free(response);
        send_error_to_client(client_socket, STATUS_ERR_SERVER_ERROR);
        return;
    }

    // Prepare the message to be sent to all the clients
    char response_message[MAX_MESSAGE_SIZE];
    snprintf(response_message, MAX_MESSAGE_SIZE, "--%s has left--", request->username);

    // Prepare the information response to be broadcasted to all clients
    response->type = RESPONSE_INFO;
    response->status = STATUS_INFO_USER_LEFT;
    snprintf(response->message, MAX_MESSAGE_SIZE, "%s", response_message);

    // Clear the response buffer and update the length
    memset(response_buffer, 0, MAX_PAYLOAD_SIZE);
    ptr = response_buffer;
    length = response_length(response);

    // Encode the response into the response buffer
    if((error = encode_response(&ptr, MAX_PAYLOAD_SIZE, length, response)) != ERR_OK){
        fprintf(stderr, "Response encoding error!\n");
        fprintf(stderr, "%s", error_to_string(error));
        fprintf(stderr, "\n");
        fflush(stderr);
        free(response);
        return;
    }

    // Send the response to all the clients
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
    // Allocate memory for the response
    Response *response = (Response *)malloc(sizeof(*response));
    if(response == NULL){
        fprintf(stderr, "Memory allocation for response failed!\n");
        fprintf(stderr, "(error = %d) : %s\n", errno, strerror(errno));
        fflush(stderr);
        return;
    }

    // Prepare the error response
    response->type = RESPONSE_ERR;
    response->status = status;

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
    if((error = send_all(client_socket, response_buffer, length + HEADER_SIZE)) != ERR_OK){
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