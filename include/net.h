#ifndef NET_H
#define NET_H

#include <stddef.h>
#include <stdint.h>

// Function to read the first 4 bytes of an incoming message from the socket and interpret it as the length of the payload
uint32_t read_payload_len(int sockfd);

// Function to write the request length into a request buffer
void write_payload_len(uint8_t *buffer, uint32_t len);

// Functions to receive and send entire messages using TCP sockets to and from a buffer
int recv_all(int sockfd, uint8_t *buffer, uint32_t length);
int send_all(int sockfd, const uint8_t *buffer, uint32_t length);

#endif