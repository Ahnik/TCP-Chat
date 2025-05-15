#include "unity.h"
#include "message.h"
#include "error.h"
#include <stdlib.h>

void setUp(void){}

void tearDown(void){}

void test_encode_request(void){
    Request request = {REQUEST_MSG, "Alice", "Hello there!"};
    uint32_t length = 22;
    size_t buffer_capacity = length + sizeof(length) + 1;
    uint8_t *buffer = (uint8_t *)calloc(buffer_capacity, sizeof(*buffer));
    
    TEST_ASSERT_EQUAL(ERR_OK, encode_request(&buffer, buffer_capacity, length, &request));
    buffer[buffer_capacity-1] = '\0';
    char *payload = (char *)(buffer + sizeof(length));
    TEST_ASSERT_EQUAL_STRING("MSG|Alice|Hello there!", payload);
    free(buffer);

    request.type = REQUEST_JOIN;
    length = 10;
    buffer_capacity = length + sizeof(length) + 1;
    uint8_t *new_buffer = (uint8_t *)calloc(buffer_capacity, sizeof(*new_buffer));

    TEST_ASSERT_EQUAL(ERR_OK, encode_request(&new_buffer, buffer_capacity, length, &request));
    new_buffer[buffer_capacity-1] = '\0';
    payload = (char *)(new_buffer + sizeof(length));
    TEST_ASSERT_EQUAL_STRING("JOIN|Alice", payload);
    free(new_buffer);

    request.type = REQUEST_MSG;
    length = 22;
    buffer_capacity = length;
    uint8_t *newer_buffer = (uint8_t *)calloc(buffer_capacity, sizeof(*newer_buffer));

    TEST_ASSERT_EQUAL(ERR_BUFFER_SIZE_EXCEEDED, encode_request(&newer_buffer, buffer_capacity, length, &request));
    free(newer_buffer);
}

void test_decode_request(void){
    Request request;
    char *payload = "MSG|Alice|Hello there!";
    TEST_ASSERT_EQUAL(ERR_OK, decode_request(&request, payload));
    TEST_ASSERT_EQUAL(REQUEST_MSG, request.type);
    TEST_ASSERT_EQUAL_STRING("Alice", request.username);
    TEST_ASSERT_EQUAL_STRING("Hello there!", request.message);

    payload = "NAME|Alice|Bob";
    TEST_ASSERT_EQUAL(ERR_OK, decode_request(&request, payload));
    TEST_ASSERT_EQUAL(REQUEST_NAME, request.type);
    TEST_ASSERT_EQUAL_STRING("Alice", request.username);
    TEST_ASSERT_EQUAL_STRING("Bob", request.message);

    payload = "JOIN|Alice";
    TEST_ASSERT_EQUAL(ERR_OK, decode_request(&request, payload));
    TEST_ASSERT_EQUAL(REQUEST_JOIN, request.type);
    TEST_ASSERT_EQUAL_STRING("Alice", request.username);

    payload = "MSG|Alice";
    TEST_ASSERT_EQUAL(ERR_INVALID, decode_request(&request, payload));

    payload = "INVALID|Alice";
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

void test_encode_response(void){
    Response response = {RESPONSE_INFO, STATUS_INFO_MESSAGE, "Alice:Hello there!"};
    uint32_t length = 31;
    uint32_t buffer_capacity = length + sizeof(length) + 1;
    uint8_t *buffer = (uint8_t *)calloc(buffer_capacity, sizeof(*buffer));
    TEST_ASSERT_EQUAL(ERR_OK, encode_response(&buffer, buffer_capacity, length, &response));
    buffer[buffer_capacity-1] = '\0';
    char *payload = (char *)(buffer + sizeof(length));
    TEST_ASSERT_EQUAL_STRING("INFO|MESSAGE|Alice:Hello there!", payload);
    free(buffer);
    
    response.type = RESPONSE_ERR;
    response.status = STATUS_ERR_NAME_MISSING;
    length = 16;
    buffer_capacity = length + sizeof(length) + 1;
    uint8_t *new_buffer = (uint8_t *)calloc(buffer_capacity, sizeof(*new_buffer));
    TEST_ASSERT_EQUAL(ERR_OK, encode_response(&new_buffer, buffer_capacity, length, &response));
    payload = (char *)(new_buffer + sizeof(length));
    TEST_ASSERT_EQUAL_STRING("ERR|NAME_MISSING", payload);
    free(new_buffer);

    response.type = RESPONSE_INFO;
    response.status = STATUS_INFO_MESSAGE;
    length = 31;
    buffer_capacity = 31;
    uint8_t *newer_buffer = (uint8_t *)calloc(buffer_capacity, sizeof(*newer_buffer));
    TEST_ASSERT_EQUAL(ERR_BUFFER_SIZE_EXCEEDED, encode_response(&newer_buffer, buffer_capacity, length, &response));
    free(newer_buffer);
}

void test_decode_response(void){
    Response response;
    char *payload = "INFO|MESSAGE|Alice:Hello there!";
    TEST_ASSERT_EQUAL(ERR_OK, decode_response(&response, payload));
    TEST_ASSERT_EQUAL(RESPONSE_INFO, response.type);
    TEST_ASSERT_EQUAL(STATUS_INFO_MESSAGE, response.status);
    TEST_ASSERT_EQUAL_STRING("Alice:Hello there!", response.message);

    payload = "INFO|MESSAGE|";
    TEST_ASSERT_EQUAL(ERR_INVALID, decode_response(&response, payload));

    payload = "ACK|NAME_SET";
    TEST_ASSERT_EQUAL(ERR_OK, decode_response(&response, payload));
    TEST_ASSERT_EQUAL(RESPONSE_ACK, response.type);
    TEST_ASSERT_EQUAL(STATUS_ACK_NAME_SET, response.status);

    payload = "ACK";
    TEST_ASSERT_EQUAL(ERR_INVALID, decode_response(&response, payload));

    payload = "ACK|INVALID";
    TEST_ASSERT_EQUAL(ERR_INVALID, decode_response(&response, payload));

    payload = "INFO|USER_JOINED|";
    TEST_ASSERT_EQUAL(ERR_INVALID, decode_response(&response, payload));

    payload = "INFO|USER_LEFT|Alice";
    TEST_ASSERT_EQUAL(ERR_OK, decode_response(&response, payload));
    TEST_ASSERT_EQUAL(RESPONSE_INFO, response.type);
    TEST_ASSERT_EQUAL(STATUS_INFO_USER_LEFT, response.status);
    TEST_ASSERT_EQUAL_STRING("Alice", response.message);

    payload = "INVALID|Alice|Joined";
    TEST_ASSERT_EQUAL(ERR_INVALID, decode_response(&response, payload));
}

int main(void){
    UNITY_BEGIN();
    RUN_TEST(test_encode_request);
    RUN_TEST(test_decode_request);
    RUN_TEST(test_encode_response);
    RUN_TEST(test_decode_response);
    return UNITY_END();
}