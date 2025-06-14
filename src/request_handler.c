#include "request_handler.h"
#include "clients.h"
#include "message.h"
#include "net.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <errno.h>

void handle_name_request(int client_socket, const Request *request){
    Client *client = find_client_by_socket(client_socket);
    Response *response = (Response *)malloc(sizeof(*response));

    if(response == NULL){
        fprintf(stderr, "Memory allocation for response failed!\n");
        fprintf(stderr, "(error = %d) : %s\n", errno, strerror(errno));
        fflush(stderr);
        return;
    }

    // Store the previous username of the client 
    char prev_name[MAX_USERNAME_LEN];
    snprintf(prev_name, MAX_USERNAME_LEN, "%s", client->username);

    // Change the name of the client 
    snprintf(client->username, MAX_USERNAME_LEN, "%s", request->message);

    // Prepare response for the client
    response->type = RESPONSE_ACK;
    response->status = STATUS_ACK_NAME_SET;

    // Prepare the message buffer to be sent to the client
    uint32_t length = strlen(response_type_to_string(response->type)) + strlen(response_status_to_string(response->status)) + DELIMITER_LENGTH;
    uint8_t response_buffer[MAX_PAYLOAD_SIZE];
    bzero(response_buffer, MAX_PAYLOAD_SIZE);
    uint8_t *ptr = response_buffer;     // Pointer to the first element of the buffer

    // Encode the response into the response buffer
    int error;
    if((error = encode_response(&ptr, MAX_PAYLOAD_SIZE, length, response)) != ERR_OK){
        fprintf(stderr, "Response encoding error!\n");
        fprintf(stderr, error_to_string(error));
        fprintf(stderr, "\n");
        fflush(stderr);
        free(response);
        return;
    }

    // Send the message to the client
    if((error = send_all(client_socket, &response_buffer, length)) != ERR_OK){
        fprintf(stderr, "Response sending error!\n");
        fprintf(stderr, error_to_string(error));
        fprintf(stderr, "\n");
        fflush(stderr);
        free(response);
        return;
    }

    // Prepare response  to be broadcasted to all clients
    response->type = RESPONSE_INFO;
    response->status = STATUS_INFO_NAME_CHANGED;
    snprintf(response->message, MAX_MESSAGE_SIZE, "--%s has changed their name to %s--", prev_name, client->username);

    // Clear the response buffer and update the length
    bzero(response_buffer, MAX_PAYLOAD_SIZE);
    ptr = response_buffer;
    length = strlen(response_type_to_string(response->type)) + strlen(response_status_to_string(response->status)) + strlen(response->message) + 2*DELIMITER_LENGTH;

    // Encode the response into the response buffer
    if((error = encode_response(&ptr, MAX_PAYLOAD_SIZE, length, response)) != ERR_OK){
        fprintf(stderr, "Response encoding error!\n");
        fprintf(stderr, error_to_string(error));
        fprintf(stderr, "\n");
        fflush(stderr);
        free(response);
        return;
    }

    // Send the response to all the clients
    if((error = broadcast_message((char *)&response_buffer)) != ERR_OK){
        fprintf(stderr, "Unable to send all messages\n");
        fprintf(stderr, error_to_string(error));
        fprintf(stderr, "\n");
        fflush(stderr);
        free(response);
        return;
    }

    free(response);
}

void handle_msg_request(int client_socket, const Request *request){

}

void handle_join_request(int client_socket, const Request *request){

}

void handle_leave_request(int client_socket, const Request *request){

}

void handle_invalid_request(int client_socket){

}