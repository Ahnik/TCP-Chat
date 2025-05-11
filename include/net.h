#ifndef NET_H
#define NET_H

#include <stddef.h>
#include <stdint.h>

// Function to read the first 4 bytes of an incoming request from the client socket and interpret it as the length of the request
uint32_t read_request_len(int sockfd);

// Function to write the request length into a request buffer
void write_request_len(uint8_t *buffer, uint32_t len);

// Functions to receive and send entire messages using TCP sockets to and from a buffer
int recv_request(int sockfd, uint8_t *buffer, uint32_t length);
int send_request(int sockfd, const uint8_t *buffer, uint32_t length);

#endif