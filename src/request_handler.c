#include "request_handler.h"
#include "clients.h"
#include "message.h"
#include "net.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>

void handle_name_request(int client_socket, const Request *request){
    Client *client = find_client_by_socket(client_socket);
    Response *response = (Response *)malloc(sizeof(*response));

    // Change the name of the client 
    snprintf(client->username, MAX_USERNAME_LEN, "%s", request->message);

    // Prepare response for the client
    response->type = RESPONSE_ACK;
    response->status = STATUS_ACK_NAME_SET;

    // Prepare the message buffer to be sent to the client
    uint32_t length = strlen(response_type_to_string(response->type)) + strlen(response_status_to_string(response->status)) + DELIMITER_LENGTH;
    uint8_t response_buffer[MAX_PAYLOAD_SIZE];
    uint8_t *ptr = response_buffer;     // Pointer to the first element of the buffer
    bzero(response_buffer, MAX_PAYLOAD_SIZE);

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

    // Prepare response to be broadcasted to all clients
    

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