#ifndef CLIENT_CONTEXT_H
#define CLIENT_CONTEXT_H

#include "common.h"
#include <pthread.h>

typedef struct{
    char username[MAX_USERNAME_SIZE];
    int socketfd;
    pthread_t input_thread;
    pthread_t receiver_thread;
    pthread_mutex_t lock;
    pthread_cond_t cond_var;
} ClientContext;

void init_client_context(ClientContext *client_context, int socketfd, const char *username);
void destroy_client_context(ClientContext *client_context);

#endif