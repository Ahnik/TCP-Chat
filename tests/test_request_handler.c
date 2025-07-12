#include "unity.h"
#include "request_handler.h"
#include "clients.h"
#include "message.h"
#include "net.h"
#include "common.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>

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

        char prev_name[MAX_USERNAME_SIZE];
        snprintf(prev_name, MAX_USERNAME_SIZE, "%s", client->username);

        request->type = REQUEST_NAME;
        snprintf(request->username, MAX_USERNAME_SIZE, "%s", client->username);
        snprintf(request->message, MAX_MESSAGE_SIZE, "%s", name);

        handle_name_request(socket_pairs[i][0], request);

        client = find_client_by_socket(socket_pairs[i][0]);
        TEST_ASSERT_NOT_NULL(client);

        TEST_ASSERT_EQUAL_STRING(name, client->username);

        char response_buffer[MAX_PAYLOAD_SIZE];
        bzero(response_buffer, MAX_PAYLOAD_SIZE);

        uint32_t length = read_payload_len(socket_pairs[i][1]);
        TEST_ASSERT_EQUAL_UINT32((uint32_t)(strlen("ACK|NAME_SET")), length);
        TEST_ASSERT_EQUAL_INT(ERR_OK, recv_all(socket_pairs[i][1], (uint8_t *)response_buffer, length));
        response_buffer[length] = 0;
        TEST_ASSERT_EQUAL_STRING("ACK|NAME_SET", response_buffer);

        Response *response = (Response *)malloc(sizeof(*response));
        TEST_ASSERT_EQUAL_INT(ERR_OK, decode_response(response, response_buffer));
        TEST_ASSERT_EQUAL(RESPONSE_ACK, response->type);
        TEST_ASSERT_EQUAL(STATUS_ACK_NAME_SET, response->status);

        for(int j=0; j<MAX_CLIENTS; j++){
            bzero(response, sizeof(*response));
            bzero(response_buffer, MAX_PAYLOAD_SIZE);

            char payload[MAX_PAYLOAD_SIZE];
            snprintf(payload, MAX_PAYLOAD_SIZE, "INFO|NAME_CHANGED|--%s has changed their name to %s--", prev_name, name);

            length = read_payload_len(socket_pairs[j][1]);
            TEST_ASSERT_EQUAL_UINT32((uint32_t)(strlen(payload)), length);
            TEST_ASSERT_EQUAL_INT(ERR_OK, recv_all(socket_pairs[j][1], (uint8_t *)response_buffer, length));
            response_buffer[length] = 0;
            TEST_ASSERT_EQUAL_STRING(payload, response_buffer);

            TEST_ASSERT_EQUAL_INT(ERR_OK, decode_response(response, response_buffer));
            TEST_ASSERT_EQUAL(RESPONSE_INFO, response->type);
            TEST_ASSERT_EQUAL(STATUS_INFO_NAME_CHANGED, response->status);
        }

        free(response);
        free(request);
        name[0]++;
    }
}

void test_handle_msg_request(){
    int socket_pairs[MAX_CLIENTS][2];   
    char name[2] = "a";
    for(int i=0; i<MAX_CLIENTS; i++){
        if(socketpair(AF_UNIX, SOCK_STREAM, 0, socket_pairs[i]) != 0) TEST_IGNORE();
        if(add_client(socket_pairs[i][0], name) != ERR_OK) TEST_IGNORE();   // 0 is for sending, 1 is for receiving
        name[0]++;
    }

    char msg[2] = "a";
    for(int i=0; i<MAX_CLIENTS; i++){
        Request *request = (Request *)malloc(sizeof(*request));
        if(request == NULL) TEST_IGNORE();
        Client *client = find_client_by_socket(socket_pairs[i][0]);
        if(client == NULL) TEST_IGNORE();

        request->type = REQUEST_MSG;
        snprintf(request->username, MAX_USERNAME_SIZE, "%s", client->username);
        snprintf(request->message, MAX_MESSAGE_SIZE, "%s", msg);

        handle_msg_request(socket_pairs[i][0], request);

        char response_buffer[MAX_PAYLOAD_SIZE];
        bzero(response_buffer, MAX_PAYLOAD_SIZE);

        uint32_t length = read_payload_len(socket_pairs[i][1]);
        TEST_ASSERT_EQUAL_UINT32((uint32_t)(strlen("ACK|OK")), length);
        TEST_ASSERT_EQUAL_INT(ERR_OK, recv_all(socket_pairs[i][1], (uint8_t *)response_buffer, length));
        response_buffer[length] = 0;
        TEST_ASSERT_EQUAL_STRING("ACK|OK", response_buffer);

        Response *response = (Response *)malloc(sizeof(*response));
        TEST_ASSERT_EQUAL_INT(ERR_OK, decode_response(response, response_buffer));
        TEST_ASSERT_EQUAL(RESPONSE_ACK, response->type);
        TEST_ASSERT_EQUAL(STATUS_ACK_OK, response->status);

        for(int j=0; j<MAX_CLIENTS; j++){
            bzero(response, sizeof(*response));
            bzero(response_buffer, MAX_PAYLOAD_SIZE);

            char payload[MAX_PAYLOAD_SIZE];
            snprintf(payload, MAX_PAYLOAD_SIZE, "INFO|MESSAGE|%s: %s", client->username, msg);

            length = read_payload_len(socket_pairs[j][1]);
            TEST_ASSERT_EQUAL_UINT32((uint32_t)(strlen(payload)), length);
            TEST_ASSERT_EQUAL_INT(ERR_OK, recv_all(socket_pairs[j][1], (uint8_t *)response_buffer, length));
            response_buffer[length] = 0;
            TEST_ASSERT_EQUAL_STRING(payload, response_buffer);

            TEST_ASSERT_EQUAL_INT(ERR_OK, decode_response(response, response_buffer));
            TEST_ASSERT_EQUAL(RESPONSE_INFO, response->type);
            TEST_ASSERT_EQUAL(STATUS_INFO_MESSAGE, response->status);
        }

        free(response);
        free(request);
    }
}

void test_handle_join_request(){
    int socket_pairs[MAX_CLIENTS][2];   
    for(int i=0; i<MAX_CLIENTS; i++){
        if(socketpair(AF_UNIX, SOCK_STREAM, 0, socket_pairs[i]) != 0) TEST_IGNORE();
    }

    char name[2] = "a";
    for(int i=0; i<MAX_CLIENTS; i++){
        Request *request = (Request *)malloc(sizeof(*request));
        request->type = REQUEST_JOIN;
        snprintf(request->username, MAX_USERNAME_SIZE, "%s", name);

        handle_join_request(socket_pairs[i][0], request);

        char response_buffer[MAX_PAYLOAD_SIZE];
        bzero(response_buffer, MAX_PAYLOAD_SIZE);

        uint32_t length = read_payload_len(socket_pairs[i][1]);
        TEST_ASSERT_EQUAL_UINT32((uint32_t)(strlen("ACK|JOINED")), length);
        TEST_ASSERT_EQUAL_INT(ERR_OK, recv_all(socket_pairs[i][1], (uint8_t *)response_buffer, length));
        response_buffer[length] = 0;
        TEST_ASSERT_EQUAL_STRING("ACK|JOINED", response_buffer);

        Response *response = (Response *)malloc(sizeof(*response));
        TEST_ASSERT_EQUAL_INT(ERR_OK, decode_response(response, response_buffer));
        TEST_ASSERT_EQUAL(RESPONSE_ACK, response->type);
        TEST_ASSERT_EQUAL(STATUS_ACK_JOINED, response->status);

        TEST_ASSERT_EQUAL_INT(i+1, count_clients());
        TEST_ASSERT_NOT_NULL(find_client_by_socket(socket_pairs[i][0]));

        for(int j=0; j<=i; j++){
            bzero(response_buffer, MAX_PAYLOAD_SIZE);
            bzero(response, sizeof(*response));

            char payload[MAX_PAYLOAD_SIZE];
            snprintf(payload, MAX_PAYLOAD_SIZE, "INFO|USER_JOINED|--%s has joined--", name);

            length = read_payload_len(socket_pairs[j][1]);
            TEST_ASSERT_EQUAL_UINT32((uint32_t)(strlen(payload)), length);
            TEST_ASSERT_EQUAL_INT(ERR_OK, recv_all(socket_pairs[j][1], (uint8_t *)response_buffer, length));
            response_buffer[length] = 0;
            TEST_ASSERT_EQUAL_STRING(payload, response_buffer);

            TEST_ASSERT_EQUAL_INT(ERR_OK, decode_response(response, response_buffer));
            TEST_ASSERT_EQUAL(RESPONSE_INFO, response->type);
            TEST_ASSERT_EQUAL(STATUS_INFO_USER_JOINED, response->status);
        }

        free(response);
        free(request);
        name[0]++;
    }
}

void test_handle_leave_request(){
    int socket_pairs[MAX_CLIENTS][2];   
    char name[2] = "a";
    for(int i=0; i<MAX_CLIENTS; i++){
        if(socketpair(AF_UNIX, SOCK_STREAM, 0, socket_pairs[i]) != 0) TEST_IGNORE();
        if(add_client(socket_pairs[i][0], name) != ERR_OK) TEST_IGNORE();   // 0 is for sending, 1 is for receiving
        name[0]++;
    }

    name[0] = 'a';
    for(int i=0; i<MAX_CLIENTS; i++){
        Request *request = (Request *)malloc(sizeof(*request));
        request->type = REQUEST_LEAVE;
        snprintf(request->username, MAX_MESSAGE_SIZE, "%s", name);

        handle_leave_request(socket_pairs[i][0], request);

        char response_buffer[MAX_PAYLOAD_SIZE];
        bzero(response_buffer, MAX_PAYLOAD_SIZE);

        uint32_t length = read_payload_len(socket_pairs[i][1]);
        TEST_ASSERT_EQUAL_UINT32((uint32_t)(strlen("ACK|QUIT")), length);
        TEST_ASSERT_EQUAL_INT(ERR_OK, recv_all(socket_pairs[i][1], (uint8_t *)response_buffer, length));
        response_buffer[length] = 0;
        TEST_ASSERT_EQUAL_STRING("ACK|QUIT", response_buffer);

        Response *response = (Response *)malloc(sizeof(*response));
        TEST_ASSERT_EQUAL_INT(ERR_OK, decode_response(response, response_buffer));
        TEST_ASSERT_EQUAL(RESPONSE_ACK, response->type);
        TEST_ASSERT_EQUAL(STATUS_ACK_QUIT, response->status);

        TEST_ASSERT_EQUAL_INT(MAX_CLIENTS-i-1, count_clients());
        TEST_ASSERT_NULL(find_client_by_socket(socket_pairs[i][0]));

        for(int j=i+1; j<MAX_CLIENTS; j++){
            bzero(response_buffer, MAX_PAYLOAD_SIZE);
            bzero(response, sizeof(*response));

            char payload[MAX_PAYLOAD_SIZE];
            snprintf(payload, MAX_PAYLOAD_SIZE, "INFO|USER_LEFT|--%s has left--", name);

            length = read_payload_len(socket_pairs[j][1]);
            TEST_ASSERT_EQUAL_UINT32((uint32_t)(strlen(payload)), length);
            TEST_ASSERT_EQUAL_INT(ERR_OK, recv_all(socket_pairs[j][1], (uint8_t *)response_buffer, length));
            response_buffer[length] = 0;
            TEST_ASSERT_EQUAL_STRING(payload, response_buffer);

            TEST_ASSERT_EQUAL_INT(ERR_OK, decode_response(response, response_buffer));
            TEST_ASSERT_EQUAL(RESPONSE_INFO, response->type);
            TEST_ASSERT_EQUAL(STATUS_INFO_USER_LEFT, response->status);
        }

        free(response);
        free(request);
        name[0]++;
    }
}

int main(){
    UNITY_BEGIN();
    RUN_TEST(test_handle_name_request);
    RUN_TEST(test_handle_msg_request);
    RUN_TEST(test_handle_join_request);
    RUN_TEST(test_handle_leave_request);
    return UNITY_END();
}