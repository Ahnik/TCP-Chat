#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <pthread.h>
#include <stdbool.h>

// The ThreadSlot structure to represent a slot on the thread pool
typedef struct{
    pthread_t thread;
    bool in_use;
    int socketfd;
    int id;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} ThreadSlot;

// The main thread function for each of the worker threads
void *thread_function(void *arg);

// Function to handle the client on the given client socket
int handle_client(int socketfd);

#endif