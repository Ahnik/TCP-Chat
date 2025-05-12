#ifndef PROTOCOL_H
#define PROTOCOL_H

// The types of requests supported
typedef enum{
    REQUEST_MSG = 0,    
    REQUEST_JOIN, 
    REQUEST_LEAVE,
    REQUEST_NAME,
    REQUEST_COUNT
} RequestType;

// The types of responses supported
typedef enum{
    RESPONSE_ACK = 0,
    RESPONSE_ERR,
    RESPONSE_INFO,
    RESPONSE_COUNT
} ResponseType;

// The types of response statuses supported
typedef enum{
    // Status for ACK responses
    STATUS_ACK_OK = 0,
    STATUS_ACK_NAME_SET,
    STATUS_ACK_JOINED,
    STATUS_ACK_QUIT,

    // Status for ERR responses
    STATUS_ERR_INVALID_CMD,
    STATUS_ERR_MALFORMED,
    STATUS_ERR_NAME_TAKEN,
    STATUS_ERR_NAME_MISSING,
    STATUS_ERR_NOT_LOGGED_IN,

    // Status for INFO responses
    STATUS_INFO_USER_JOINED,
    STATUS_INFO_USER_LEFT,
    STATUS_INFO_MESSAGE,
    STATUS_INFO_SERVER_SHUTDOWN,
    STATUS_COUNT
} ResponseStatus;

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

// Functions to map strings to the corresponding enums 
RequestType parse_request_type(const char *type_str);
ResponseType parse_response_type(const char *type_str);
ResponseStatus parse_response_status(const char *status_str);

// Functions to map the enums to corresponding strings
const char *request_type_to_string(RequestType type);
const char *response_type_to_string(ResponseType type);
const char *response_status_to_string(ResponseStatus status);

#endif