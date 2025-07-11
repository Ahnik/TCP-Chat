#include "common.h"
#include "protocol.h"
#include "message.h"
#include "client_threads.h"
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

// Mutex lock and condition variable
static pthread_mutex_t lock;
static pthread_cond_t condition_var;

int main(int argc, char **argv){
    // Usage check
    if(argc != 2) exit_with_error("Usage: %s <server address>", argv[0]);

    // Create the client socket
    int client_socket;
    if((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        exit_with_error("Failed to create socket!");

    // Initialize and fill up the server address struct
    struct sockaddr_in server_addr;
    memset((void *)&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(SERVER_PORT);

    // Convert server address from text to binary form and store it in server_addr.sin_addr
    if(inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0)
        exit_with_error("inet_pton error for %s", argv[1]);

    // Connect to the server address
    if(connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        exit_with_error("Connection failed!");

    // Initialize the mutex lock and conditional variable
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&condition_var, NULL);

    // Initialize the input and output threads
    pthread_t input_thread, output_thread;

    // Create the input and output threads
    pthread_create(&input_thread , NULL, input_thread_function , NULL);
    pthread_create(&output_thread, NULL, output_thread_function, NULL);

    // Terminate the input and output threads
    pthread_join(input_thread , NULL);
    pthread_join(output_thread, NULL);

    // Destroy the mutex lock and conditional variable
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&condition_var);

    close(client_socket);
    return 0;
}