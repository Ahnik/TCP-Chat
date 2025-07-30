#include "input.h"
#include "client_context.h"
#include "send_request.h"
#include "receive_response.h"
#include "protocol.h"
#include "common.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

void *input_thread_function(void *arg){
    ClientContext *client_context = (ClientContext *)arg;
    while(client_context->running){
        char input_buffer[MAX_MESSAGE_SIZE];
        if(fgets(input_buffer, MAX_MESSAGE_SIZE, stdin) == NULL){
            close(client_context->socketfd);
            fprintf(stderr, "Client input error!\n");
            fprintf(stderr, "(errno = %d) : %s\n", errno, strerror(errno));
            break;
        }
        input_buffer[strcspn(input_buffer, "\n")] = '\0';

        pthread_mutex_lock(&client_context->lock);
        if(strncmp(input_buffer, QUIT_PROMPT, strlen(QUIT_PROMPT)) != 0){
            send_request(client_context->socketfd, REQUEST_MSG, client_context->username, input_buffer);

            Response *response = receive_response(client_context->socketfd);
            if(response == NULL){
                close(client_context->socketfd);
                pthread_mutex_unlock(&client_context->lock);
                break;
            }

            if(response->type != RESPONSE_ACK || response->status != STATUS_ACK_OK){
                if(response->type == RESPONSE_ERR && response->status == STATUS_ERR_NOT_LOGGED_IN){
                    fprintf(stderr, "Client is not logged into the server!\nShutting down client!\n");
                    fflush(stderr);
                    free(response);
                    pthread_mutex_unlock(&client_context->lock);
                    break;
                }else{
                    fprintf(stderr, "Invalid response from server!\nShutting down client!\n");
                    fflush(stderr);
                    free(response);
                    pthread_mutex_unlock(&client_context->lock);
                    break;
                }
            }
            free(response);
        }else{
            send_request(client_context->socketfd, REQUEST_LEAVE, client_context->username, "");
            client_context->running = false;
        }
        pthread_mutex_unlock(&client_context->lock);
    }
    return NULL;
}