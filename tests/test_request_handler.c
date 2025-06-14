#include "unity.h"
#include "request_handler.h"
#include "clients.h"
#include "message.h"
#include "common.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>

// Unity Test Setup/Teardown functions
void setUp(){
    cleanup_clients();
}

void tearDown(){
    cleanup_clients();
}

void test_handle_name_request(){
    int socket_pairs[MAX_CLIENTS][2];   
    char name[2] = "a";
    for(int i=0; i<MAX_CLIENTS; i++){
        if(socketpair(AF_UNIX, SOCK_STREAM, 0, socket_pairs[i]) != 0) TEST_IGNORE();
        if(add_client(socket_pairs[i][0], name) != ERR_OK) TEST_IGNORE();   // 0 is for sending, 1 is for receiving
        name[0]++;
    }

    for(int i=0; i<MAX_CLIENTS; i++){
        Request *request = (Request *)malloc(sizeof(*request));
        if(request == NULL) TEST_IGNORE();
        Client *client = find_client_by_socket(socket_pairs[i][0]);
        if(client == NULL) TEST_IGNORE();

        request->type = REQUEST_NAME;
        snprintf(request->username, MAX_USERNAME_LEN, "%s", client->username);
        snprintf(request->message, MAX_MESSAGE_SIZE, "%s", name);

        handle_name_request(socket_pairs[i][0], request);

        free(request);
        name[0]++;
    }
}

int main(){
    UNITY_BEGIN();
    return UNITY_END();
}