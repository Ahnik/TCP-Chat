#include "unity.h"
#include "message.h"
#include "error.h"
#include <stdlib.h>
#include <string.h>

void setUp(){}

void tearDown(){}

void test_encode_request_msg(){
    Request request = {REQUEST_MSG, "Alice", "Hello there!"};
    uint32_t length = 22;
    size_t buffer_capacity = length + sizeof(length) + 1;
    uint8_t *buffer = (uint8_t *)calloc(buffer_capacity, sizeof(*buffer));
    
    TEST_ASSERT_EQUAL(ERR_OK, encode_request(&buffer, buffer_capacity, length, &request));
    buffer[buffer_capacity-1] = '\0';
    char *payload = (char *)(buffer + sizeof(length));
    TEST_ASSERT_EQUAL_STRING("MSG|Alice|Hello there!", payload);
    free(buffer);
}

void test_encode_request_join(){
    Request request = {REQUEST_JOIN, "Alice", ""};
    uint32_t length = 10;
    size_t buffer_capacity = length + sizeof(length) + 1;
    uint8_t *buffer = (uint8_t *)calloc(buffer_capacity, sizeof(*buffer));

    TEST_ASSERT_EQUAL(ERR_OK, encode_request(&buffer, buffer_capacity, length, &request));
    buffer[buffer_capacity-1] = '\0';
    char *payload = (char *)(buffer + sizeof(length));
    TEST_ASSERT_EQUAL_STRING("JOIN|Alice", payload);
    free(buffer);
}

void test_encode_request_leave(){
    Request request = {REQUEST_LEAVE, "Alice", ""};
    uint32_t length = 11;
    size_t buffer_capacity = length + sizeof(length) + 1;
    uint8_t *buffer = (uint8_t *)calloc(buffer_capacity, sizeof(*buffer));

    TEST_ASSERT_EQUAL(ERR_OK, encode_request(&buffer, buffer_capacity, length, &request));
    buffer[buffer_capacity-1] = '\0';
    char *payload = (char *)(buffer + sizeof(length));
    TEST_ASSERT_EQUAL_STRING("LEAVE|Alice", payload);
    free(buffer);
}

void test_encode_request_name(){
    Request request = {REQUEST_NAME, "Alice", "Bob"};
    uint32_t length = 14;
    size_t buffer_capacity = length + sizeof(length) + 1;
    uint8_t *buffer = (uint8_t *)calloc(buffer_capacity, sizeof(*buffer));

    TEST_ASSERT_EQUAL(ERR_OK, encode_request(&buffer, buffer_capacity, length, &request));
    buffer[buffer_capacity-1] = '\0';
    char *payload = (char *)(buffer + sizeof(length));
    TEST_ASSERT_EQUAL_STRING("NAME|Alice|Bob", payload);
    free(buffer);
}

void test_encode_request_invalid(){
    Request request = {REQUEST_NAME, "Alice", "Bob"};
    uint32_t length = 14;
    size_t buffer_capacity = 14;
    uint8_t *buffer = (uint8_t *)calloc(buffer_capacity, sizeof(*buffer));

    TEST_ASSERT_EQUAL(ERR_BUFFER_SIZE_EXCEEDED, encode_request(&buffer, buffer_capacity, length, &request));
    free(buffer);
}

void test_decode_request_msg(){
    Request request;
    char *payload = "MSG|Alice|Hello there!";
    TEST_ASSERT_EQUAL(ERR_OK, decode_request(&request, payload));
    TEST_ASSERT_EQUAL(REQUEST_MSG, request.type);
    TEST_ASSERT_EQUAL_STRING("Alice", request.username);
    TEST_ASSERT_EQUAL_STRING("Hello there!", request.message);
}

void test_decode_request_join(){
    Request request;
    char *payload = "JOIN|Alice";
    TEST_ASSERT_EQUAL(ERR_OK, decode_request(&request, payload));
    TEST_ASSERT_EQUAL(REQUEST_JOIN, request.type);
    TEST_ASSERT_EQUAL_STRING("Alice", request.username);
}

void test_decode_request_leave(){
    Request request;
    char *payload = "LEAVE|Alice";
    TEST_ASSERT_EQUAL(ERR_OK, decode_request(&request, payload));
    TEST_ASSERT_EQUAL(REQUEST_LEAVE, request.type);
    TEST_ASSERT_EQUAL_STRING("Alice", request.username);
}

void test_decode_request_name(){
    Request request;
    char *payload = "NAME|Alice|Bob";
    TEST_ASSERT_EQUAL(ERR_OK, decode_request(&request, payload));
    TEST_ASSERT_EQUAL(REQUEST_NAME, request.type);
    TEST_ASSERT_EQUAL_STRING("Alice", request.username);
    TEST_ASSERT_EQUAL_STRING("Bob", request.message);
}

void test_decode_request_invalid(){
    Request request;
    char *payload = "INVALID|Alice";
    TEST_ASSERT_EQUAL(ERR_INVALID, decode_request(&request, payload));

    payload = "JOIN";
    TEST_ASSERT_EQUAL(ERR_INVALID, decode_request(&request, payload));

    payload = "INVALID";
    TEST_ASSERT_EQUAL(ERR_INVALID, decode_request(&request, payload));

    payload = "LEAVE|Alice";
    TEST_ASSERT_EQUAL(ERR_OK, decode_request(&request, payload));

    payload = "LEAVE|";
    TEST_ASSERT_EQUAL(ERR_INVALID, decode_request(&request, payload));
}

void test_encode_response_ack(){
    Response response = {RESPONSE_ACK, STATUS_ACK_OK, ""};
    uint32_t length = strlen("ACK|OK");
    uint32_t buffer_capacity = length + sizeof(length) + 1;
    uint8_t *buffer = (uint8_t *)calloc(buffer_capacity, sizeof(*buffer));
    TEST_ASSERT_EQUAL(ERR_OK, encode_response(&buffer, buffer_capacity, length, &response));
    buffer[buffer_capacity-1] = '\0';
    char *payload = (char *)(buffer + sizeof(length));
    TEST_ASSERT_EQUAL_STRING("ACK|OK", payload);
    free(buffer);
}

void test_encode_response_err(){
    Response response = {RESPONSE_ERR, STATUS_ERR_INVALID_CMD, ""};
    uint32_t length = strlen("ERR|INVALID_CMD");
    uint32_t buffer_capacity = length + sizeof(length) + 1;
    uint8_t *buffer = (uint8_t *)calloc(buffer_capacity, sizeof(*buffer));
    TEST_ASSERT_EQUAL(ERR_OK, encode_response(&buffer, buffer_capacity, length, &response));
    buffer[buffer_capacity-1] = '\0';
    char *payload = (char *)(buffer + sizeof(length));
    TEST_ASSERT_EQUAL_STRING("ERR|INVALID_CMD", payload);
    free(buffer);    
}

void test_encode_response_info(){
    Response response = {RESPONSE_INFO, STATUS_INFO_MESSAGE, "Alice:Hello there!"};
    uint32_t length = 31;
    uint32_t buffer_capacity = length + sizeof(length) + 1;
    uint8_t *buffer = (uint8_t *)calloc(buffer_capacity, sizeof(*buffer));
    TEST_ASSERT_EQUAL(ERR_OK, encode_response(&buffer, buffer_capacity, length, &response));
    buffer[buffer_capacity-1] = '\0';
    char *payload = (char *)(buffer + sizeof(length));
    TEST_ASSERT_EQUAL_STRING("INFO|MESSAGE|Alice:Hello there!", payload);
    free(buffer);
}

void test_encode_response_invalid(){
    Response response = {RESPONSE_INFO, STATUS_INFO_MESSAGE, "Alice:Hi"};
    uint32_t length = 31;
    uint32_t buffer_capacity = 17;
    uint8_t *buffer = (uint8_t *)calloc(buffer_capacity, sizeof(*buffer));
    TEST_ASSERT_EQUAL(ERR_BUFFER_SIZE_EXCEEDED, encode_response(&buffer, buffer_capacity, length, &response));
    free(buffer);
}

void test_decode_response_ack(){
    Response response;
    char *payload = "ACK|QUIT";
    TEST_ASSERT_EQUAL(ERR_OK, decode_response(&response, payload));
    TEST_ASSERT_EQUAL(RESPONSE_ACK, response.type);
    TEST_ASSERT_EQUAL(STATUS_ACK_QUIT, response.status);
}

void test_decode_response_err(){
    Response response;
    char *payload = "ERR|NAME_TAKEN";
    TEST_ASSERT_EQUAL(ERR_OK, decode_response(&response, payload));
    TEST_ASSERT_EQUAL(RESPONSE_ERR, response.type);
    TEST_ASSERT_EQUAL(STATUS_ERR_NAME_TAKEN, response.status);
}

void test_decode_response_info(){
    Response response;
    char *payload = "INFO|NAME_CHANGED|Alice has changed name to Bob";
    TEST_ASSERT_EQUAL(ERR_OK, decode_response(&response, payload));
    TEST_ASSERT_EQUAL(RESPONSE_INFO, response.type);
    TEST_ASSERT_EQUAL(STATUS_INFO_NAME_CHANGED, response.status);
    TEST_ASSERT_EQUAL_STRING("Alice has changed name to Bob", response.message);
}

void test_decode_response_invalid(){
    Response response;
    char *payload = "INFO|MESSAGE|";
    TEST_ASSERT_EQUAL(ERR_INVALID, decode_response(&response, payload));

    payload = "ACK";
    TEST_ASSERT_EQUAL(ERR_INVALID, decode_response(&response, payload));

    payload = "ACK|INVALID";
    TEST_ASSERT_EQUAL(ERR_INVALID, decode_response(&response, payload));

    payload = "INFO|USER_JOINED|";
    TEST_ASSERT_EQUAL(ERR_INVALID, decode_response(&response, payload));

}

int main(void){
    UNITY_BEGIN();
    RUN_TEST(test_encode_request_msg);
    RUN_TEST(test_encode_request_join);
    RUN_TEST(test_encode_request_leave);
    RUN_TEST(test_encode_request_name);
    RUN_TEST(test_encode_request_invalid);

    RUN_TEST(test_decode_request_msg);
    RUN_TEST(test_decode_request_join);
    RUN_TEST(test_decode_request_leave);
    RUN_TEST(test_decode_request_name);
    RUN_TEST(test_decode_response_invalid);

    RUN_TEST(test_encode_response_ack);
    RUN_TEST(test_encode_response_err);
    RUN_TEST(test_encode_response_info);
    RUN_TEST(test_encode_response_invalid);

    RUN_TEST(test_decode_response_ack);
    RUN_TEST(test_decode_response_err);
    RUN_TEST(test_decode_response_info);
    RUN_TEST(test_decode_response_invalid);

    return UNITY_END();
}