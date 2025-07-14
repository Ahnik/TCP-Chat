#include "receive_response.h"
#include "message.h"
#include "net.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

Response *receive_response(int socketfd){
    // Receive the length of the payload
    uint32_t payload_length = read_payload_len(socketfd);
    if(payload_length == 0){
        fprintf(stderr, "Reading response length failed!\n");
        fflush(stderr);
        return NULL;
    }

    // Create the payload buffer to store the incoming client request
    uint8_t *payload_buffer = (uint8_t *)calloc(payload_length+1, sizeof(*payload_buffer));
    if(payload_buffer == NULL){
        fprintf(stderr, "Memory allocation for payload buffer failed!\n");
        fflush(stderr);
        return NULL;
    }

    // Receive the server response 
    int error;
    if((error = recv_all(socketfd, payload_buffer, payload_length)) != ERR_OK){
        fprintf(stderr, "Receiving from server failed!\n");
        fprintf(stderr, "%s", error_to_string(error));
        fprintf(stderr, "\n");
        fflush(stderr);
        free(payload_buffer);
        return NULL;
    }
    payload_buffer[payload_length] = '\0';

    // Allocate memory for the response struct
    Response *response = (Response *)malloc(sizeof(*response));
    if(response == NULL){
        fprintf(stderr, "Memory allocation for response failed!\n");
        fflush(stderr);
        free(payload_buffer);
        return NULL;
    }

    // Decode the response
    if((error = decode_response(response, (char *)payload_buffer)) != ERR_OK){
        fprintf(stderr, "Decoding response error!\n");
        fprintf(stderr, "%s", error_to_string(error));
        fprintf(stderr, "\n");
        fflush(stderr);
        free(payload_buffer);
        free(response);
        return NULL;
    }

    return response;
}