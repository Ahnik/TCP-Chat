#include "net.h"
#include "common.h"
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

uint32_t read_payload_len(int sockfd){
    if(sockfd < 0) return 0;
    uint32_t *len_buf = (uint32_t *)calloc(1, sizeof(*len_buf));
    if(!len_buf) return 0;
    ssize_t bytesWritten = 0;
    while(bytesWritten < HEADER_SIZE){
        ssize_t bytesReceived = recv(sockfd, len_buf + bytesWritten, HEADER_SIZE - bytesWritten, 0);
        if(bytesReceived < 0)       return UINT32_MAX;
        else if(bytesReceived == 0) return 0;
        bytesWritten += bytesReceived;
    }
    uint32_t length = ntohl(*len_buf);
    free(len_buf);
    return length;
}

int recv_all(int sockfd, uint8_t *buffer, uint32_t length){
    if(sockfd < 0)     return ERR_INVALID;
    if(buffer == NULL) return ERR_INVALID;
    if(length == 0)    return ERR_INVALID;
    ssize_t bytesWritten = 0;
    while(bytesWritten < length){
        ssize_t bytesReceived = recv(sockfd, buffer + bytesWritten, length - bytesWritten, 0);
        if(bytesReceived < 0)       return ERR_RECEIVING;
        else if(bytesReceived == 0) return ERR_CONNECTION_CLOSED;
        bytesWritten += bytesReceived;
    }
    return ERR_OK;
}

int send_all(int sockfd, const uint8_t *buffer, uint32_t length){
    if(sockfd < 0)     return ERR_INVALID;
    if(buffer == NULL) return ERR_INVALID;
    if(length == 0)    return ERR_INVALID;
    ssize_t total_bytes = 0;
    while(total_bytes < length){
        ssize_t bytesSent = send(sockfd, buffer + total_bytes, length - total_bytes, 0);
        if(bytesSent <= 0) return ERR_RECEIVING;
        total_bytes += bytesSent;
    }
    return ERR_OK;
}