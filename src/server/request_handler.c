#include "request_handler.h"
#include "clients.h"
#include "message.h"
#include "net.h"
#include "protocol.h"
#include "send_response.h"
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

    // Store the previous username of the client 
    char prev_name[MAX_USERNAME_SIZE];
    snprintf(prev_name, MAX_USERNAME_SIZE, "%s", client->username);

    // Change the name of the client 
    snprintf(client->username, MAX_USERNAME_SIZE, "%s", request->message);
    
    // Send the acknowledgement response to the client
    send_response_to_client(client_socket, RESPONSE_ACK, STATUS_ACK_NAME_SET, "");
    
    // Prepare the message to be sent and broadcast it to all clients
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, MAX_MESSAGE_SIZE, "--%s has changed their name to %s--", prev_name, client->username);
    broadcast_response(RESPONSE_INFO, STATUS_INFO_NAME_CHANGED, message);
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
    
    // Send acknowledgement response to the client
    send_response_to_client(client_socket, RESPONSE_ACK, STATUS_ACK_OK, "");

    // Prepare message to be broadcasted
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, MAX_MESSAGE_SIZE, "%s: %s", request->username, request->message);

    // Broadcast message to all connected clients
    broadcast_response(RESPONSE_INFO, STATUS_INFO_MESSAGE, message);
}

void handle_join_request(int client_socket, const Request *request){
    // Check if the there is already a client by that name
    if(find_client_by_name(request->username) != NULL){
        send_error_to_client(client_socket, STATUS_ERR_NAME_TAKEN);
        return;
    }

    // Join the client
    int error;
    if((error = add_client(client_socket, request->username)) != ERR_OK){
        fprintf(stderr, "Unable to add client!\n");
        fprintf(stderr, "%s", error_to_string(error));
        fprintf(stderr, "\n");
        fflush(stderr);
        send_error_to_client(client_socket, STATUS_ERR_SERVER_ERROR);
        return;
    }
    
    // Send the acknowledgement response to the client
    send_response_to_client(client_socket, RESPONSE_ACK, STATUS_ACK_JOINED, "");

    // Prepare the message to be broadcasted
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, MAX_MESSAGE_SIZE, "--%s has joined--", request->username);

    // Broadcast the response to all the clients
    broadcast_response(RESPONSE_INFO, STATUS_INFO_USER_JOINED, message);
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

    // Remove the client
    int error;
    if((error = remove_client(client_socket)) != ERR_OK){
        fprintf(stderr, "Unable to remove client!\n");
        fprintf(stderr, "\n");
        fprintf(stderr, "%s", error_to_string(error));
        fflush(stderr);
        send_error_to_client(client_socket, STATUS_ERR_SERVER_ERROR);
        return;
    }
    
    // Send acknowledgement response the client
    send_response_to_client(client_socket, RESPONSE_ACK, STATUS_ACK_QUIT, "");

    // Prepare the message to be broadcasted
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, MAX_MESSAGE_SIZE, "--%s has left--", request->username);

    // Broadcast the response to all the clients
    broadcast_response(RESPONSE_INFO, STATUS_INFO_USER_LEFT, message);
}