#include "protocol.h"
#include <string.h>

RequestType parse_request_type(const char *type_str){
    if(strcmp(type_str, "MSG") == 0)
        return REQUEST_MSG;
    else if(strcmp(type_str, "JOIN") == 0)
        return REQUEST_JOIN;
    else if(strcmp(type_str, "LEAVE") == 0)
        return REQUEST_LEAVE;
    else if(strcmp(type_str, "NAME") == 0)  
        return REQUEST_NAME;
    else
        return REQUEST_COUNT;
}

ResponseType parse_response_type(const char *type_str){
    if(strcmp(type_str, "ACK") == 0)
        return RESPONSE_ACK;
    else if(strcmp(type_str, "ERR") == 0)
        return RESPONSE_ERR;
    else if(strcmp(type_str, "INFO") == 0)
        return RESPONSE_INFO;
    else
        return RESPONSE_COUNT;
}

ResponseStatus parse_response_status(const char *status_str){
    for(size_t i=0; i<sizeof(status_table)/sizeof(status_table[0]); i++){
        if(strcmp(status_str, status_table[i].str) == 0)
            return status_table[i].status;
    }

    return STATUS_COUNT;
}

const char *request_type_to_string(RequestType type){
    switch(type){
        case REQUEST_MSG:
            return "MSG";
        case REQUEST_JOIN:
            return "JOIN";
        case REQUEST_LEAVE:
            return "LEAVE";
        case REQUEST_NAME:
            return "NAME";
        default:
            return NULL;
    }
}

const char *response_type_to_string(ResponseType type){
    switch(type){
        case RESPONSE_ACK:
            return "ACK";
        case RESPONSE_ERR:
            return "ERR";
        case RESPONSE_INFO:
            return "INFO";
        default:
            return NULL;
    }
}

const char *response_status_to_string(ResponseStatus status){
    for(size_t i=0; i<sizeof(status_table)/sizeof(status_table[0]); i++){
        if(status == status_table[i].status)
            return status_table[i].str;
    }

    return NULL;
}