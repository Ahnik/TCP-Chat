#include "unity.h"
#include "clients.h"
#include <pthread.h>

#define N 10

static pthread_mutex_t mutex;

void setUp(){
    pthread_mutex_init(&mutex, NULL);
}

void tearDown(){
    cleanup_clients();
    pthread_mutex_destroy(&mutex);
}

void test_add_client_empty_list(){
    
}

int main(){
    UNITY_BEGIN();
    return UNITY_END();
}