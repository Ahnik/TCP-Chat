#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include "message.h"

// Function to handle NAME requests
void handle_name_request(int client_socket, const Request *request);

// Function to handle MSG requests
void handle_msg_request(int client_socket, const Request *request);

// Function to handle JOIN requests
void handle_join_request(int client_socket, const Request *request);

// Function to handle LEAVE requests
void handle_leave_request(int client_socket, const Request *request);

// Function to handle invalid requests
void handle_invalid_request(int client_socket);

#endif