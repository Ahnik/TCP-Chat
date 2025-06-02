#include "unity.h"
#include "clients.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define N 12
#define M N

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

// Thread function for randomized mixed access
void *mixed_access_thread(void *arg){
    char username[32];
    int id = *(int *)arg;

    for(int i=0; i<M*N; i++){
        snprintf(username, sizeof(username), "user_%d_%d", id, i);
        switch(rand() % 3){
            case 0:
                add_client(id*M*N + i, username);
                break;
            case 1:
                find_client_by_name(username);
                break;
            case 2:
                remove_client(id*M*N + i);
                break;
        }
    }
    return NULL;
}

// Testing concurrent additions to the client list
void test_add_client(){
    pthread_t thread_pool[N];
    add_client_args_t *args[N];
    char str[2] = "a";

    for(int i=0; i<N; i++){
        if((args[i] = (add_client_args_t *)malloc(sizeof(add_client_args_t))) == NULL) TEST_IGNORE();
        if((args[i]->username = strdup(str)) == NULL) TEST_IGNORE();
        args[i]->socketfd = i+1;
        str[0]++;
        TEST_ASSERT_EQUAL_INT(0, pthread_create(&thread_pool[i], NULL, add_client_thread, (void *)args[i]));
    }

    for(int i=0; i<N; i++){
        TEST_ASSERT_EQUAL_INT(0, pthread_join(thread_pool[i], NULL));
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
        if((add_args[i] = (add_client_args_t *)malloc(sizeof(add_client_args_t))) == NULL) TEST_IGNORE();
        if((add_args[i]->username = strdup(str)) == NULL) TEST_IGNORE();
        add_args[i]->socketfd = i+1;
        TEST_ASSERT_EQUAL_INT(0, pthread_create(&thread_pool[i], NULL, add_client_thread, (void *)add_args[i]));
    }

    for(int i=0; i<N; i++){
        TEST_ASSERT_EQUAL_INT(0, pthread_join(thread_pool[i], NULL));
    }

    for(int i=0; i<N; i++){
        int j = (i+2)%N;
        if(j == 0) j = N;
        if((remove_args[i] = (remove_client_args_t *)malloc(sizeof(remove_client_args_t))) == NULL) TEST_IGNORE();
        remove_args[i]->socketfd = j;
        TEST_ASSERT_EQUAL_INT(0, pthread_create(&thread_pool[i], NULL, remove_client_thread, (void *)remove_args[i]));
    }    

    for(int i=0; i<N; i++){
        TEST_ASSERT_EQUAL_INT(0, pthread_join(thread_pool[i], NULL));
    }

    for(int i=0; i<N; i++){
        free(add_args[i]->username);
        free(add_args[i]);
        free(remove_args[i]);
    }

    TEST_ASSERT_EQUAL_INT(0, count_clients());

    str[0] = 'a';
    for(int i=0; i<N; i++){
        str[0]++;
        Client *client = find_client_by_name(str);
        TEST_ASSERT_NULL(client);
    }
}

// Testing concurrent additions and removals in the client list
void test_concurrent_add_and_remove(){
    pthread_t thread_pool[4*N/3];
    add_client_args_t *add_args[N];
    remove_client_args_t *remove_args[N/3];
    char str[2] = "a";

    for(int i=0; i<N; i++){
        if((add_args[i] = (add_client_args_t *)malloc(sizeof(add_client_args_t))) == NULL) TEST_IGNORE();
        add_args[i]->socketfd = i+1;
        if((add_args[i]->username = strdup(str)) == NULL) TEST_IGNORE();
        TEST_ASSERT_EQUAL_INT(0, pthread_create(&thread_pool[i], NULL, add_client_thread, (void *)add_args[i]));
        str[0]++;
    }

    for(int i=0; i<N/3; i++){
        if((remove_args[i] = (remove_client_args_t *)malloc(sizeof(remove_client_args_t))) == NULL) TEST_IGNORE();
        remove_args[i]->socketfd = (i+1)*3;
        TEST_ASSERT_EQUAL_INT(0, pthread_create(&thread_pool[N+i], NULL, remove_client_thread, (void *)remove_args[i]));
    }

    for(int i=0; i<4*N/3; i++){
        TEST_ASSERT_EQUAL_INT(0, pthread_join(thread_pool[i], NULL));
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

// Testing concurrent access to the clients list
void test_concurrent_access(){
    char str[2] = "a";
    for(int i=0; i<N; i++){
        if(add_client(i+1, str) != ERR_OK) TEST_IGNORE();
        str[0]++;
    }

    pthread_t access_thread[N];
    find_client_args_t *find_args[N];
    Client *clients[N];
    str[0] = 'a';

    for(int i=0; i<N; i++){
        int j = (i+3)%N;
        if(j == 0) j = N;
        if((find_args[i] = (find_client_args_t *)malloc(sizeof(find_client_args_t))) == NULL) TEST_IGNORE();
        if((find_args[i]->username = strdup(str)) == NULL) TEST_IGNORE();
        TEST_ASSERT_EQUAL_INT(0, pthread_create(&access_thread[i], NULL, find_client_thread, (void *)find_args[i]));
        str[0]++;
    }

    str[0] = 'a';
    for(int i=0; i<N; i++){
        TEST_ASSERT_EQUAL_INT(0, pthread_join(access_thread[i], (void **)&clients[i]));
        TEST_ASSERT_NOT_NULL(clients[i]);
        TEST_ASSERT_EQUAL_INT(i+1, clients[i]->socketfd);
        TEST_ASSERT_EQUAL_STRING(str, clients[i]->username);
        free(find_args[i]->username);
        free(find_args[i]);
        str[0]++;
    }
}

// Testing for high-workload concurrent access
void test_heavy_concurrent_access(){
    char str[2] = "a";
    for(int i=0; i<N; i++){
        if(add_client(i+1, str) != ERR_OK) TEST_IGNORE();
        str[0]++;
    }

    pthread_t access_thread[M*N];
    find_client_args_t *find_args[N];
    Client *clients[M];
    str[0] = 'a';

    for(int i=0; i<N; i++){
        int k=(i+4)%N;
        if(k == 0) k = N;
        if((find_args[i] = (find_client_args_t *)malloc(sizeof(find_client_args_t))) == NULL) TEST_IGNORE();
        if((find_args[i]->username = strdup(str)) == NULL) TEST_IGNORE();
        for(int j=0; j<M; j++){
            TEST_ASSERT_EQUAL_INT(0, pthread_create(&access_thread[M*i + j], NULL, find_client_thread, (void *)find_args[i]));
        }
        str[0]++;
    }

    str[0] = 'a';
    for(int i=0; i<N; i++){
        for(int j=0; j<M; j++){
            TEST_ASSERT_EQUAL_INT(0, pthread_join(access_thread[M*i + j], (void **)&clients[N*i + j]));
            TEST_ASSERT_NOT_NULL(clients[M*i + j]);
            TEST_ASSERT_EQUAL_INT(i+1, clients[M*i + j]->socketfd);
            TEST_ASSERT_EQUAL_STRING(str, clients[M*i + j]->username);
        }
        free(find_args[i]->username);
        free(find_args[i]);
        str[0]++;
    }
}

// Testing for mixed access, addition and removal in the client list
void test_concurrent_mixed_access(){
    pthread_t thread_pool[N];
    int ids[N];

    for(int i=0; i<N; i++){
        ids[i] = i;
        TEST_ASSERT_EQUAL_INT(0, pthread_create(&thread_pool[i], NULL, mixed_access_thread, (void *)&ids[i]));
    }

    for(int i=0; i<N; i++){
        TEST_ASSERT_EQUAL_INT(0, pthread_join(thread_pool[i], NULL));
    }

    for(int i=0; i<N; i++){
        char username[32];
        for(int j=0; j<M*N; j++){
            snprintf(username, sizeof(username), "user_%d_%d", ids[i], i);
            Client *client = find_client_by_name(username);
            if(client){
                TEST_ASSERT_EQUAL_INT(ids[i]*M*N+i, client->socketfd);
                TEST_ASSERT_EQUAL_INT(ERR_OK, remove_client(ids[i]*M*N+i));
                TEST_ASSERT_NULL(find_client_by_name(username));
            }
        }
    }
}

int main(){
    UNITY_BEGIN();
    RUN_TEST(test_add_client);
    RUN_TEST(test_remove_client);
    RUN_TEST(test_concurrent_add_and_remove);
    RUN_TEST(test_concurrent_access);
    RUN_TEST(test_heavy_concurrent_access);
    RUN_TEST(test_concurrent_mixed_access);
    return UNITY_END();
}