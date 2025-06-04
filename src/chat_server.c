#include "client_handler.h"
#include "common.h"
#include "net.h"
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
 
// Declaring the pool of thread slots to handle the clients
static ThreadSlot *thread_slots[MAX_CLIENTS];

int main(int argc, char **argv){
    // Spawning the threads
    for(int i=0; i<MAX_CLIENTS; i++){
        // Allocate memory for the thread slots
        if((thread_slots[i] = (ThreadSlot *)malloc(sizeof(ThreadSlot))) == NULL)
            exit_with_error("Unable to allocate memory for threads!");

        pthread_create(&thread_slots[i]->thread, NULL, thread_function, (void *)thread_slots[i]);
        thread_slots[i]->in_use = false;
        thread_slots[i]->id = i;
        thread_slots[i]->socketfd = -1;
        pthread_mutex_init(&thread_slots[i]->lock, NULL);
        pthread_cond_init(&thread_slots[i]->cond, NULL);
    }

    // Create the server socket
    int server_socket;
    if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        exit_with_error("Failed to create socket!");
    
    // Initialize the address struct
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    // Bind the server address to the server socket
    if(bind(server_socket, &server_addr, sizeof(server_addr)) == -1)
        exit_with_error("Bind Failed!");

    while(true){
        printf("Waiting for connection...\n");
        int client_socket;
        struct sockaddr_in client_addr;
        socklen_t addr_size = (socklen_t)sizeof(struct sockaddr_in);

        // Accept the client
        client_socket = accept(server_socket, &client_addr, &addr_size);
        if(client_socket == -1){
            printf("Accept Failed!\n");
            continue;
        }

        bool slot_available = false;
        // Once the client is accepted, the server will look for any available thread slot.
        for(int i=0; i<MAX_CLIENTS; i++){
            pthread_mutex_lock(&thread_slots[i]->lock);
            // Check if the thread slot is available
            if(thread_slots[i]->in_use == false){
                slot_available = true;
                thread_slots[i]->in_use = true;
                thread_slots[i]->socketfd = client_socket;
                pthread_cond_signal(&thread_slots[i]->cond);
                pthread_mutex_unlock(&thread_slots[i]->lock);
                break;
            }
            pthread_mutex_unlock(&thread_slots[i]->lock);
        }

        if(slot_available){
            char message[29] = "You have successfully joined";
            send(client_socket, message, sizeof(message), 0);
        }else{
            char message[24] = "The chat server is full";
            send(client_socket, message, sizeof(message), 0);
        }
    }

    return 0;
}