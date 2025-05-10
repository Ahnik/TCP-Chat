#ifndef NET_H
#define NET_H

#include <stddef.h>
#include <stdint.h>

// Function to read the first 4 bytes of the request and interpret it as the request length
uint32_t read_request_len(const uint8_t *buffer);

// Function to write the request length into a request buffer
void write_request_len(uint8_t *buffer, uint32_t len);

// Functions to receive and send entire messages using TCP sockets
int recv_all(int sock_fd, void *buffer, size_t length);
int send_all(int sock_fd, const void *buffer, size_t length);

#endif