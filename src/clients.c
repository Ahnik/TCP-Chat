#include "clients.h"
#include "common.h"
#include <stdlib.h>
#include <pthread.h>

// Declaration of the head of the client list
static Client *client_list_head = NULL;

// Declaration of the tail of the client list
static Client *client_list_tail = NULL;

// Declaration of the mutex lock
static pthread_mutex_t client_list_mutex = PTHREAD_MUTEX_INITIALIZER;

int add_client(int socketfd, const char *username){
    if(client_list_head){

    }else{
        Client *client = (Client *)malloc(sizeof(*client));
        if(!client) return ERR_MEMORY;
    }
    return ERR_OK;
}