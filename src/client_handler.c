#include "client_handler.h"
#include "net.h"
#include "message.h"
#include "protocol.h"
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
    bool leave = false; // Boolean variable to determine if the client wants to leave or not
    while(leave){
        // Read the payload length from the client
        uint32_t payload_length = read_payload_len(client_socket);
        if(payload_length == 0){
            fprintf(stderr, "Reading request length failed!\n");
            fflush(stderr);
            continue;
        }

        // Creating the payload buffer to store the incoming client request
        uint8_t *payload_buffer = (uint8_t *)calloc(payload_length+1, sizeof(*payload_buffer));
        if(payload_buffer == NULL){
            fprintf(stderr, "Memory allocation for payload buffer failed!\n");
            fflush(stderr);
            continue;
        }

        // Receiving the client request 
        int error;
        if((error = recv_all(client_socket, payload_buffer, payload_length)) != ERR_OK){
            fprintf(stderr, "Receiving from client failed!\n");
            fprintf(stderr, error_to_string(error));
            fprintf(stderr, "\n");
            fflush(stderr);
            free(payload_buffer);
            continue;
        }

        // Null-terminate the buffer to read it as a string of characters
        payload_buffer[payload_length] = '\0';

        // Allocate memory for the Request struct
        Request *request = (Request *)malloc(sizeof(*request));
        if(request == NULL){
            fprintf(stderr, "Memory allocation for request failed!\n");
            fflush(stderr);
            free(payload_buffer);
            continue;
        }

        // Decode the request 
        if((error = decode_request(request, payload_buffer)) != ERR_OK){
            fprintf(stderr, "Decoding reqeust error!\n");
            fprintf(stderr, error_to_string(error));
            fprintf(stderr, "\n");
            fflush(stderr);
            free(payload_buffer);
            free(request);
            continue;
        }

        
    }

    // Close the client socket
    close(client_socket);
}