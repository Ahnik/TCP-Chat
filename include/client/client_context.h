#ifndef CLIENT_CONTEXT_H
#define CLIENT_CONTEXT_H

#include <pthread.h>

typedef struct{
    int socketfd;
    pthread_t input_thread;
    pthread_t receiver_thread;
    pthread_mutex_t lock;
    pthread_cond_t cond_var;
} ClientContext;

void init_client_context(ClientContext *client_context, int socketfd);
void destroy_client_context(ClientContext *client_context);

#endif