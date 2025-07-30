#include "receiver.h"
#include "client_context.h"
#include "message.h"
#include "receive_response.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

void *receiver_thread_function(void *arg){
    ClientContext *client_context = (ClientContext *)arg;
    printf("\033[H\033[J");
    printf("\n> ");
    fflush(stdout);

    bool quit = false;
    while(!quit){
        Response *response = receive_response(client_context->socketfd);
        if(response == NULL){
            close(client_context->socketfd);
            break;
        }

        pthread_mutex_lock(&client_context->lock);
        if(response->type == RESPONSE_INFO){
            // Move up, place the cursor at the beginning of the line and clear the line and up again
            printf("\033[A\r\033[K\033[A");
            printf("%s\n", response->message);
            printf("\n> ");
            fflush(stdout);
        }

        if(client_context->running == false)
            quit = true;
        pthread_mutex_unlock(&client_context->lock);
        free(response);
    }
    return NULL;
}