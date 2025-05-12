#include "unity.h"
#include "protocol.h"
#include <string.h>

// Creating a lookup table of status codes and their corresponding strings 
struct StatusEntry{
    const char *str;
    ResponseStatus status;
};

static const struct StatusEntry status_table[] = {
    {"OK"             , STATUS_ACK_OK},
    {"NAME_SET"       , STATUS_ACK_NAME_SET},
    {"JOINED"         , STATUS_ACK_JOINED},
    {"QUIT"           , STATUS_ACK_QUIT},
    {"INVALID_CMD"    , STATUS_ERR_INVALID_CMD},
    {"MALFORMED"      , STATUS_ERR_MALFORMED},
    {"NAME_MISSING"   , STATUS_ERR_NAME_MISSING},
    {"NAME_TAKEN"     , STATUS_ERR_NAME_TAKEN},
    {"NOT_LOGGED_IN"  , STATUS_ERR_NOT_LOGGED_IN},
    {"USER_JOINED"    , STATUS_INFO_USER_JOINED},
    {"USER_LEFT"      , STATUS_INFO_USER_LEFT},
    {"MESSAGE"        , STATUS_INFO_MESSAGE},
    {"SERVER_SHUTDOWN", STATUS_INFO_SERVER_SHUTDOWN}
};

void test_parse_request_type(void){
    TEST_ASSERT_EQUAL(REQUEST_MSG   , parse_request_type("MSG"));
    TEST_ASSERT_EQUAL(REQUEST_JOIN  , parse_request_type("JOIN"));
    TEST_ASSERT_EQUAL(REQUEST_LEAVE , parse_request_type("LEAVE")); 
    TEST_ASSERT_EQUAL(REQUEST_NAME  , parse_request_type("NAME"));
    TEST_ASSERT_EQUAL(REQUEST_COUNT , parse_request_type("INVALID"));
}

void test_parse_response_type(void){
    TEST_ASSERT_EQUAL(RESPONSE_ACK  , parse_response_type("ACK"));
    TEST_ASSERT_EQUAL(RESPONSE_ERR  , parse_response_type("ERR"));
    TEST_ASSERT_EQUAL(RESPONSE_INFO , parse_response_type("INFO"));
    TEST_ASSERT_EQUAL(RESPONSE_COUNT, parse_response_type("INVALID"));
}

void test_parse_response_status(void){
    for(size_t i=0; i<sizeof(status_table)/sizeof(status_table[0]); i++){
        TEST_ASSERT_EQUAL(status_table[i].status, parse_response_status(status_table[i].str));
    }
}

void test_request_type_to_string(void){
    TEST_ASSERT_EQUAL_STRING("MSG"  , request_type_to_string(REQUEST_MSG));
    TEST_ASSERT_EQUAL_STRING("JOIN" , request_type_to_string(REQUEST_JOIN));
    TEST_ASSERT_EQUAL_STRING("LEAVE", request_type_to_string(REQUEST_LEAVE));
    TEST_ASSERT_EQUAL_STRING("NAME" , request_type_to_string(REQUEST_NAME));
}

void test_response_type_to_string(void){
    TEST_ASSERT_EQUAL_STRING("ACK" , response_type_to_string(RESPONSE_ACK));
    TEST_ASSERT_EQUAL_STRING("ERR" , response_type_to_string(RESPONSE_ERR));
    TEST_ASSERT_EQUAL_STRING("INFO", response_type_to_string(RESPONSE_INFO));
}

void test_response_status_to_string(void){
    for(size_t i=0; i<sizeof(status_table)/sizeof(status_table[0]); i++){
        TEST_ASSERT_EQUAL_STRING(status_table[i].str, response_status_to_string(status_table[i].status));
    }
}

int main(void){
    UNITY_BEGIN();
    RUN_TEST(test_parse_request_type);
    RUN_TEST(test_parse_response_type);
    RUN_TEST(test_parse_response_status);
    RUN_TEST(test_request_type_to_string);
    RUN_TEST(test_response_type_to_string);
    RUN_TEST(test_response_status_to_string);
    return UNITY_END();
}