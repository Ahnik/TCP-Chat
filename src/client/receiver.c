#include "receiver.h"
#include "client_context.h"
#include "message.h"
#include "receive_response.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

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
            pthread_mutex_unlock(&client_context->lock);
            quit = true;
            continue;
        }
        pthread_mutex_unlock(&client_context->lock);

        Response *response = receive_response(client_context->socketfd);
        if(response == NULL){
            fprintf(stderr, "Shutting down the client! Press Enter to exit!\n");
            pthread_mutex_lock(&client_context->lock);
            client_context->running = false;
            pthread_mutex_unlock(&client_context->lock);
            quit = true;
            continue;
        }

        pthread_mutex_lock(&client_context->lock);
        if(response->type == RESPONSE_ERR){
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
        else if(response->type == RESPONSE_INFO){
            char *message_duplicate = strndup(response->message, MAX_MESSAGE_SIZE);
            if(message_duplicate == NULL){
                fprintf(stderr, "Client error occurred!\n");
                fprintf(stderr, "Shutting down client! Print Enter to exit!\n");
                client_context->running = false;
                pthread_mutex_unlock(&client_context->lock);
                quit = true;
                continue;
            }
            char *chatter_name = strtok(message_duplicate, ":");
            if(chatter_name == NULL){
                fprintf(stderr, "Unable to determine the sender name!\n");
                fprintf(stderr, "Shutting down client! Print Enter to exit!\n");
                client_context->running = false;
                pthread_mutex_unlock(&client_context->lock);
                quit = true;
                continue;
            }

            if(strncmp(chatter_name, client_context->username, MAX_USERNAME_SIZE) == 0)
                printf("\033[A\r\033[K\033[A");
            else
                printf("\r\033[K\033[A");
            printf("%s\n\n> ", response->message);
            fflush(stdout);
            free(message_duplicate);
        }
        pthread_mutex_unlock(&client_context->lock);
        free(response);
    }
    return NULL;
}