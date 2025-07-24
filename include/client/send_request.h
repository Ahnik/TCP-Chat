#ifndef SEND_REQUEST_H
#define SEND_REQUEST_H

#include "message.h"

// Function to send a request to a socket
void send_request(int socketfd, RequestType type, const char *username, const char *message);

#endif