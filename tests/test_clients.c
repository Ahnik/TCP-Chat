#include "unity.h"
#include "clients.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define N 12

typedef struct{
    int socketfd;
    char *username;
} add_client_args_t;

typedef struct{
    int socketfd;
} remove_client_args_t;

typedef struct{
    char *username;
} find_client_args_t;

// Unity Test Setup/Teardown functions
void setUp(){
    cleanup_clients();
}

void tearDown(){
    cleanup_clients();
}

// Thread wrapper functions
void *add_client_thread(void *arg){
    add_client_args_t *args = (add_client_args_t *)arg;
    add_client(args->socketfd, args->username);
    return NULL;
}

void *remove_client_thread(void *arg){
    remove_client_args_t *args = (remove_client_args_t *)arg;
    remove_client(args->socketfd);
    return NULL;
}

void *find_client_thread(void *arg){
    find_client_args_t *args = (find_client_args_t *)arg;
    Client *client = find_client_by_name(args->username);
    return (void *)client;
}

// Testing concurrent additions to the client list
void test_add_client(){
    pthread_t thread_pool[N];
    add_client_args_t *args[N];
    char str[2] = "a";

    for(int i=0; i<N; i++){
        args[i] = (add_client_args_t *)malloc(sizeof(add_client_args_t));
        args[i]->username = strdup(str);
        args[i]->socketfd = i+1;
        str[0]++;
        pthread_create(&thread_pool[i], NULL, add_client_thread, (void *)args[i]);
    }

    for(int i=0; i<N; i++){
        pthread_join(thread_pool[i], NULL);
    }

    for(int i=0; i<N; i++){
        free(args[i]->username);
        free(args[i]);
    }

    TEST_ASSERT_EQUAL_INT(N, count_clients());
    str[0] = 'a';
    for(int i=0; i<N; i++){
        Client *client = find_client_by_name(str);
        TEST_ASSERT_NOT_NULL(client);
        TEST_ASSERT_EQUAL_INT(i+1, client->socketfd);
        TEST_ASSERT_EQUAL_STRING(str, client->username);
        str[0]++;
    }
}

// Testing concurrent removals from the client list
void test_remove_client(){
    pthread_t thread_pool[N];
    add_client_args_t *add_args[N];
    remove_client_args_t *remove_args[N];
    char str[2] = "a";

    for(int i=0; i<N; i++){
        str[0]++;
        add_args[i] = (add_client_args_t *)malloc(sizeof(add_client_args_t));
        add_args[i]->username = strdup(str);
        add_args[i]->socketfd = i+1;
        pthread_create(&thread_pool[i], NULL, add_client_thread, (void *)add_args[i]);
    }

    for(int i=0; i<N; i++){
        pthread_join(thread_pool[i], NULL);
    }

    for(int i=0; i<N; i++){
        int j = (i+2)%N;
        if(j == 0) j = N;
        remove_args[i] = (remove_client_args_t *)malloc(sizeof(remove_client_args_t));
        remove_args[i]->socketfd = j;
        pthread_create(&thread_pool[i], NULL, remove_client_thread, (void *)remove_args[i]);
    }    

    for(int i=0; i<N; i++){
        pthread_join(thread_pool[i], NULL);
    }

    for(int i=0; i<N; i++){
        free(add_args[i]->username);
        free(add_args[i]);
        free(remove_args[i]);
    }

    TEST_ASSERT_EQUAL_INT(0, count_clients());
}

// Testing concurrent additions and removals in the client list
void test_concurrent_add_and_remove(){
    pthread_t thread_pool[4*N/3];
    add_client_args_t *add_args[N];
    remove_client_args_t *remove_args[N/3];
    char str[2] = "a";

    for(int i=0; i<N; i++){
        add_args[i] = (add_client_args_t *)malloc(sizeof(add_client_args_t));
        add_args[i]->socketfd = i+1;
        add_args[i]->username = strdup(str);
        pthread_create(&thread_pool[i], NULL, add_client_thread, (void *)add_args[i]);
        str[0]++;
    }

    for(int i=0; i<N/3; i++){
        remove_args[i] = (remove_client_args_t *)malloc(sizeof(remove_client_args_t));
        remove_args[i]->socketfd = (i+1)*3;
        pthread_create(&thread_pool[N+i], NULL, remove_client_thread, (void *)remove_args[i]);
    }

    for(int i=0; i<4*N/3; i++){
        pthread_join(thread_pool[i], NULL);
    }

    for(int i=0; i<N/3; i++){
        free(remove_args[i]);
        free(add_args[i]->username);
        free(add_args[i]);
    }

    for(int i=0; i<2*N/3; i++){
        free(add_args[N/3 + i]->username);
        free(add_args[N/3 + i]);
    }

    TEST_ASSERT_EQUAL_INT(2*N/3, count_clients());
    str[0] = 'a';
    for(int i=1; i<=N; i++){
        Client *client = find_client_by_name(str);
        if(i%3){
            TEST_ASSERT_NOT_NULL(client);
            TEST_ASSERT_EQUAL_INT(i, client->socketfd);
            TEST_ASSERT_EQUAL_STRING(str, client->username);
        }else{
            TEST_ASSERT_NULL(client);
        }
        str[0]++;
    }
}

int main(){
    UNITY_BEGIN();
    RUN_TEST(test_add_client);
    RUN_TEST(test_remove_client);
    RUN_TEST(test_concurrent_add_and_remove);
    return UNITY_END();
}