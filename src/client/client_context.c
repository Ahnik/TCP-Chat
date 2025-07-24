#include "client_context.h"
#include <pthread.h>

void init_client_context(ClientContext *client_context, int socketfd){
    client_context->socketfd = socketfd;
    pthread_mutex_init(&client_context->lock, NULL);
    pthread_mutex_init(&client_context->cond_var, NULL);
}

void destroy_client_context(ClientContext *client_context){
    pthread_mutex_destroy(&client_context->lock);
    pthread_cond_destroy(&client_context->cond_var);
}