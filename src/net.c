#include "net.h"
#include "common.h"
#include <arpa/inet.h>
#include <string.h>

uint32_t read_payload_len(int sockfd){
    uint32_t *len_buf = (uint32_t *)calloc(1, sizeof(*len_buf));
    if(!len_buf)
        return 0;
    size_t bytesWritten = 0;
    size_t bytesRequired = sizeof(uint32_t);
    while(bytesWritten < bytesRequired){
        size_t bytesReceived = recv(sockfd, len_buf + bytesWritten, bytesRequired - bytesWritten, 0);
        if(bytesReceived <= 0) 
            return 0;
        bytesWritten += bytesReceived;
    }
    uint32_t length = ntohl(*len_buf);
    free(len_buf);
    return length;
}

void write_payload_len(uint8_t *buffer, uint32_t len){
    uint32_t net_len = htonl(len);
    memcpy(buffer, &net_len, sizeof(uint32_t));
}

int recv_all(int sockfd, uint8_t *buffer, uint32_t length){
    uint32_t bytesWritten = 0;
    while(bytesWritten < length){
        uint32_t bytesReceived = recv(sockfd, buffer + bytesWritten, length - bytesWritten, 0);
        if(bytesReceived <= 0) return -1;
        bytesWritten += bytesReceived;
    }
    return 0;
}

int send_all(int sockfd, const uint8_t *buffer, uint32_t length){
    uint32_t total_bytes = 0;
    while(total_bytes < length){
        uint32_t bytesSent = send(sockfd, buffer + total_bytes, length - total_bytes, 0);
        if(bytesSent <= 0) return -1;
        total_bytes += bytesSent;
    }
    return 0;
}