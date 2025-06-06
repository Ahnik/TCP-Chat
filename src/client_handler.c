#include "client_handler.h"
#include "net.h"
#include "message.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void *thread_function(void *arg){
    ThreadSlot *thread_slot = (ThreadSlot *)arg;
    while(true){
        pthread_mutex_lock(&thread_slot->lock);
        if(thread_slot->in_use == false){
            pthread_cond_wait(&thread_slot->cond, &thread_slot->lock);
            if(thread_slot->in_use && thread_slot->socketfd != -1){
                handle_client(thread_slot->socketfd);
                thread_slot->in_use = false;
                thread_slot->socketfd = -1;
            }
        }else thread_slot->in_use = false;
        pthread_mutex_unlock(&thread_slot->lock);
    }
    return NULL;
}

void handle_client(int client_socket){
    while(true){
        // Read the request length from the client
        uint32_t reqeust_length = read_payload_len(client_socket);
        if(reqeust_length == 0){
            fprintf(stderr, "Reading request length failed!\n");
            fflush(stderr);
            continue;
        }

        // Creating the request buffer to store the incoming client request
        uint8_t *request_buffer = (uint8_t *)calloc(reqeust_length+1, sizeof(*request_buffer));
        if(request_buffer == NULL){
            fprintf(stderr, "Memory allocation for request buffer failed!\n");
            fflush(stderr);
            continue;
        }

        // Receiving the client request 
        int error;
        if((error = recv_all(client_socket, request_buffer, reqeust_length)) != ERR_OK){
            fprintf(stderr, "Receiving from client failed!\n");
            fprintf(stderr, error_to_string(error));
            fprintf(stderr, "\n");
            fflush(stderr);
            free(request_buffer);
            continue;
        }

        // Null-terminate the buffer to read it as a string of characters
        request_buffer[reqeust_length] = '\0';
    }

    // Close the client socket
    close(client_socket);
}