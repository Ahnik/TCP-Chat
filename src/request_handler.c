#include "request_handler.h"
#include "clients.h"
#include "message.h"
#include "net.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

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
    
}

void handle_msg_request(int client_socket, const Request *request){

}

void handle_join_request(int client_socket, const Request *request){

}

void handle_leave_request(int client_socket, const Request *request){

}

void handle_invalid_request(int client_socket){

}