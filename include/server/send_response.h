#ifndef SEND_RESPONSE_H
#define SEND_RESPONSE_H

#include "message.h"

// Function to send a response to a socket
void send_response_to_client(int socketfd, ResponseType type, ResponseStatus status, const char *message);

// Function to broadcast a response to all clients
void broadcast_response(ResponseType type, ResponseStatus status, const char *message);

// Function to handle invalid requests
void send_error_to_client(int client_socket, ResponseStatus status);

#endif