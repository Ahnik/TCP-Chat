#include "client_context.h"
#include <pthread.h>
#include <stdio.h>

void init_client_context(ClientContext *client_context, int socketfd, const char *username){
    snprintf(client_context->username, MAX_USERNAME_SIZE, "%s", username);
    client_context->socketfd = socketfd;
    pthread_mutex_init(&client_context->lock, NULL);
    pthread_mutex_init(&client_context->cond_var, NULL);
    client_context->running = true;
}

void destroy_client_context(ClientContext *client_context){
    pthread_mutex_destroy(&client_context->lock);
    pthread_cond_destroy(&client_context->cond_var);
}