#include "clients.h"
#include "common.h"
#include "net.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// Declaration of the head of the client list
static Client *client_list_head = NULL;

// Declaration of the tail of the client list
static Client *client_list_tail = NULL;

// Declaration of the mutex lock
static pthread_mutex_t client_list_mutex = PTHREAD_MUTEX_INITIALIZER;

int add_client(int socketfd, const char *username){
    if(socketfd < 0) return ERR_INVALID;

    pthread_mutex_lock(&client_list_mutex);
    // Allocate memory for the new client in the client list
    Client *client = (Client *)malloc(sizeof(*client));
    if(!client) return ERR_MEMORY;
    client->next = NULL;
    client->socketfd = socketfd;
    strcpy(client->username, username);

    if(client_list_tail){
        client_list_tail->next = client;
        client_list_tail = client;
    }else{
        client_list_head = client;
        client_list_tail = client;
    }

    pthread_mutex_unlock(&client_list_mutex);
    return ERR_OK;
}

int remove_client(int socketfd){
    if(socketfd < 0) return ERR_INVALID;

    pthread_mutex_lock(&client_list_mutex);
    Client *client = client_list_head;
    Client *prev_client = NULL;
    int err_code = ERR_NOT_FOUND;

    while(client){
        if(client->socketfd == socketfd){
            err_code = ERR_OK;
            if(client->socketfd == client_list_head->socketfd) client_list_head = client_list_head->next;
            else if(client->socketfd == client_list_tail->socketfd) client_list_tail = prev_client;
            if(prev_client) prev_client->next = client->next;
            free(client);
            break;
        }
        prev_client = client;
        client = client->next;
    }

    pthread_mutex_unlock(&client_list_mutex);
    return err_code;
}

int broadcast_message(const char *message){
    pthread_mutex_lock(&client_list_mutex);
    Client *client = client_list_head;
    int err_code = ERR_OK;

    while(client){
        err_code = send_all(client->socketfd, (uint8_t *)message, strlen(message));
        client = client->next;
    }

    pthread_mutex_unlock(&client_list_mutex);
    return err_code;
}

Client *find_client_by_name(const char *username){
    pthread_mutex_lock(&client_list_mutex);
    Client *client = client_list_head;

    while(client){
        if(strcmp(client->username, username) == 0) break;
        client = client->next;
    }

    pthread_mutex_unlock(&client_list_mutex);
    return client;
}

Client *find_client_by_socket(int socketfd){
    pthread_mutex_lock(&client_list_mutex);
    Client *client = client_list_head;

    while(client){
        if(client->socketfd == socketfd) break;
        client = client->next;
    }

    pthread_mutex_unlock(&client_list_mutex);
    return client;
}

int count_clients(){
    pthread_mutex_lock(&client_list_mutex);
    Client *client = client_list_head;
    int count = 0;

    while(client){
        count++;
        client = client->next;
    }

    pthread_mutex_unlock(&client_list_mutex);
    return count;
}

void cleanup_clients(){
    pthread_mutex_lock(&client_list_mutex);
    Client *client = client_list_head;
    Client *prev_client = NULL;

    while(client){
        prev_client = client;
        client = client->next;
        free(prev_client);
    }
    client_list_head = NULL;
    client_list_tail = NULL;
    pthread_mutex_unlock(&client_list_mutex);
}