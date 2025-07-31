#include "receiver.h"
#include "client_context.h"
#include "message.h"
#include "receive_response.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void *receiver_thread_function(void *arg){
    ClientContext *client_context = (ClientContext *)arg;
    printf("\033[H\033[J");
    fflush(stdout);

    bool quit = false;
    pthread_mutex_lock(&client_context->lock);
    if(!client_context->running) quit = true;
    pthread_mutex_unlock(&client_context->lock);
    
    while(!quit){
        pthread_mutex_lock(&client_context->lock);
        if(!client_context->running){
            quit = true;
            pthread_mutex_unlock(&client_context->lock);
            continue;
        }
        pthread_mutex_unlock(&client_context->lock);

        Response *response = receive_response(client_context->socketfd);
        if(response == NULL){
            printf("Shutting down the client! Press Enter to exit!\n");
            pthread_mutex_lock(&client_context->lock);
            client_context->running = false;
            pthread_mutex_unlock(&client_context->lock);
            break;
        }

        pthread_mutex_lock(&client_context->lock);
        if(response->type == RESPONSE_INFO){
            printf("\033[A\r\033[K\033[A");
            printf("%s\n", response->message);
            printf("\n> ");
            fflush(stdout);
        }
        else if(response->type == RESPONSE_ERR){
            printf("\033[A\r\033[K\033[A");
            switch(response->status){
                case STATUS_ERR_NOT_LOGGED_IN:
                    printf("You are not logged in!\n");
                    break;
                default:
                    printf("Server error occurred!\n");
            }
            printf("Shutting down client! Press Enter to exit!\n");
            client_context->running = false;
        }
        free(response);
        pthread_mutex_unlock(&client_context->lock);
    }
    return NULL;
}