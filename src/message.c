#include "message.h"
#include "protocol.h"
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

int encode_request(uint8_t **buffer, size_t buffer_capacity, uint32_t length, const Request *request){
    uint32_t net_length = htonl(length);
    char *type_str = request_type_to_string(request->type);

    if(buffer_capacity <= sizeof(net_length)) return -1;
    uint8_t *pointer = memcpy(*buffer, &net_length, sizeof(net_length));
    pointer += sizeof(net_length);
    buffer_capacity -= sizeof(net_length);

    if(buffer_capacity <= strlen(type_str)) return -1;
    pointer = memcpy(pointer, type_str, strlen(type_str));
    pointer += strlen(type_str);
    buffer_capacity -= strlen(type_str);

    if(buffer_capacity <= 1) return -1;
    pointer = memset(pointer, '|', 1);
    pointer++;
    buffer_capacity--;

    if(buffer_capacity <= strlen(request->username)) return -1;
    pointer = memcpy(pointer, request->username, strlen(request->username));
    pointer += strlen(request->username);
    buffer_capacity -= strlen(request->username);

    if(buffer_capacity <= 1) return -1;
    pointer = memset(pointer, '|', 1);
    pointer++;
    buffer_capacity--;

    if(buffer_capacity < strlen(request->message)) return -1;
    pointer = memcpy(pointer, request->message, strlen(request->message));
    return 0;
}