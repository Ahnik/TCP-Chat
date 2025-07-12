#ifndef CLIENTS_H
#define CLIENTS_H

#include "common.h"
#include <stdint.h>

// Struct to create a linked list data structure to store the client data
// The client data includes the client username and their socket file descriptor through which the server communicates with them 
typedef struct client{
    int socketfd;
    char username[MAX_USERNAME_SIZE];
    struct client *next;
} Client;

// All the functions below will use mutex locks in order to ensure thread-safe operations
// Function to add a new client to the global client list
int add_client(int socketfd, const char *username);

// Function to remove a client from the global client list
int remove_client(int socketfd);

// Function to broadcast a message to every client in the global client list
int broadcast_message(const uint8_t *message, uint32_t message_length);

// Function to find a particular client based on its username
Client *find_client_by_name(const char *username);

// Function to find a particular client based on its socket fd
Client *find_client_by_socket(int socketfd);

// Function to count the number of clients on the client list
int count_clients();

// Function to clean up the client list 
void cleanup_clients();

#endif