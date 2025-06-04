#include "client_handler.h"
#include <pthread.h>
#include <stdbool.h>

void *thread_function(void *arg){
    ThreadSlot *thread_slot = (ThreadSlot *)arg;
    while(true){
        pthread_mutex_lock(&thread_slot->lock);
        if(thread_slot->in_use == false){
            pthread_cond_wait(&thread_slot->cond, &thread_slot->lock);
        }
        pthread_mutex_unlock(&thread_slot->lock);
    }
    return NULL;
}

int handle_client(int client_socket){

}