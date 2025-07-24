#ifndef RECEIVE_RESPONSE_H
#define RECEIVE_RESPONSE_H

#include "message.h"

// Function to receive a response from the server into a buffer
Response *receive_response(int socketfd);

#endif