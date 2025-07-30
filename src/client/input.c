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
    bool quit = false;
    while(!quit){
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
        }else{
            send_request(client_context->socketfd, REQUEST_LEAVE, client_context->username, "");
            client_context->running = false;
            quit = true;
        }
        pthread_mutex_unlock(&client_context->lock);
    }
    return NULL;
}