#include "unity.h"
#include "message.h"
#include "error.h"

void setUp(void){}

void tearDown(void){}

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
    RUN_TEST(test_decode_request);
    RUN_TEST(test_decode_response);
    return UNITY_END();
}