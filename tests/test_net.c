#include "unity.h"
#include "net.h"
#include "error.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

void setUp(){}

void tearDown(){}

void test_read_payload_len(){
    int sv[2];
    if(socketpair(AF_UNIX, SOCK_STREAM, 0, sv) != 0) TEST_ABORT();
    uint32_t length = htonl(32);
    size_t total_bytes = 0;
    while(total_bytes < sizeof(length)){
        size_t bytesSent = send(sv[0], &length + total_bytes, sizeof(length) - total_bytes, 0);
        if(bytesSent <= 0) TEST_ABORT();
        total_bytes += bytesSent;
    }
    TEST_ASSERT_EQUAL(32, read_payload_len(sv[1]));

    length = htonl(UINT32_MAX);
    total_bytes = 0;
    while(total_bytes < sizeof(length)){
        size_t bytesSent = send(sv[0], &length + total_bytes, sizeof(length) - total_bytes, 0);
        if(bytesSent <= 0) TEST_ABORT();
        total_bytes += bytesSent;
    }
    TEST_ASSERT_EQUAL(UINT32_MAX, read_payload_len(sv[1]));

    length = htonl(0);
    total_bytes = 0;
    while(total_bytes < sizeof(length)){
        size_t bytesSent = send(sv[0], &length + total_bytes, sizeof(length) - total_bytes, 0);
        if(bytesSent <= 0) TEST_ABORT();
        total_bytes += bytesSent;
    }
    TEST_ASSERT_EQUAL(0, read_payload_len(sv[1]));
}

void test_recv_all(){
    int sv[2];
    if(socketpair(AF_UNIX, SOCK_STREAM, 0, sv) != 0) TEST_ABORT();
    const char *payload = "MSG|Alice|Hello there!";
    uint32_t length = strlen(payload);
    uint8_t *buffer = (uint8_t *)calloc(length+1, sizeof(*buffer));
    if(!buffer) TEST_ABORT();

    uint32_t total_bytes = 0;
    while(total_bytes < length){
        uint32_t bytesSent = send(sv[0], payload + total_bytes, length - total_bytes, 0);
        if(bytesSent <= 0) TEST_ABORT();
        total_bytes += bytesSent;
    }
    TEST_ASSERT_EQUAL(ERR_OK, recv_all(sv[1], buffer, length));
    buffer[length] = '\0';
    TEST_ASSERT_EQUAL_STRING(payload, (char *)buffer);
    free(buffer);
}

void test_send_all(){
    int sv[2];
    if(socketpair(AF_UNIX, SOCK_STREAM, 0, sv) != 0) TEST_ABORT();
    const char *payload = "MSG|Alice|Hello there!";
    uint32_t length = strlen(payload);
    uint8_t *buffer = (uint8_t *)calloc(length+1, sizeof(*buffer));
    if(!buffer) TEST_ABORT();

    TEST_ASSERT_EQUAL(ERR_OK, send_all(sv[0], (uint8_t *)payload, length));
    TEST_ASSERT_EQUAL(ERR_OK, recv_all(sv[1], buffer, length));
    buffer[length] = '\0';
    TEST_ASSERT_EQUAL_STRING(payload, (char *)buffer);
    free(buffer);
}

void test_edge_cases(){
    TEST_ASSERT_EQUAL(ERR_INVALID, recv_all(-1, NULL, 0));
    TEST_ASSERT_EQUAL(ERR_INVALID, recv_all(0, NULL, 0));
    TEST_ASSERT_EQUAL(ERR_INVALID, send_all(-1, NULL, 0));
    TEST_ASSERT_EQUAL(ERR_INVALID, send_all(0, NULL, 0));

    uint8_t *buffer = (uint8_t *)calloc(2, sizeof(*buffer));
    TEST_ASSERT_EQUAL(ERR_INVALID, recv_all(0, buffer, 0));
    TEST_ASSERT_EQUAL(ERR_INVALID, send_all(0, buffer, 0));
    free(buffer);
}

int main(){
    UNITY_BEGIN();
    RUN_TEST(test_read_payload_len);
    RUN_TEST(test_recv_all);
    RUN_TEST(test_send_all);
    RUN_TEST(test_edge_cases);
    return UNITY_END();
}