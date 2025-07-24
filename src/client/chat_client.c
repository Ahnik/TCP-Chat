#include "common.h"
#include "protocol.h"
#include "message.h"
#include "net.h"
#include "send_request.h"
#include "receive_response.h"
#include "client_context.h"
#include "input.h"
#include "receiver.h"
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdint.h>

int main(int argc, char **argv){
    // Usage check
    if(argc != 2) exit_with_error("Usage: %s <server address>", argv[0]);

    // Create the client socket
    int server_socket;
    if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        exit_with_error("Failed to create socket!");

    // Initialize and fill up the server address struct
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(SERVER_PORT);

    // Convert server address from text to binary form and store it in server_addr.sin_addr
    if(inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0)
        exit_with_error("inet_pton error for %s", argv[1]);

    // Connect to the server address
    if(connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        exit_with_error("Connection failed!");

    // Receive the acknowledgement or error response from the server
    Response *response = receive_response(server_socket);
    if(response == NULL){
        close(server_socket);
        exit(0);
    }
    
    // If the response is an error response, exit 
    if(response->type == RESPONSE_ERR && response->status == STATUS_ERR_SERVER_FULL){
        fprintf(stderr, "Server is full!\n");
        fflush(stderr);
        free(response);
        close(server_socket);
        exit(0);
    }   // The server response is invalid
    else if(response->type != RESPONSE_ACK || response->status != STATUS_ACK_OK){
        fprintf(stderr, "Invalid response from server!\n");
        fflush(stderr);
        free(response);
        close(server_socket);
        exit(0);
    }
    free(response);
    response = NULL;

    // Variable to store username
    char username[MAX_USERNAME_SIZE];

    // Clear the screen 
    printf("\033[H\033[J");

    // Until the user fills in valid name that is not taken, the loop will keep going on
    while(true){
        // Enter the name
        printf("Enter your name: ");
        if(fgets(username, MAX_USERNAME_SIZE, stdin) == NULL){
            exit_with_error("Input name error!");
            close(server_socket);
        }

        // Send JOIN request to the server
        send_request(server_socket, REQUEST_JOIN, username, "");

        // Receive the response
        response = receive_response(server_socket);
        if(response == NULL){
            close(server_socket);
            exit(0);
        }
        
        // Process the response
        if(response->type == RESPONSE_ACK && response->status == STATUS_ACK_JOINED){
            break;
        }else if(response->type == RESPONSE_ERR && response->status == STATUS_ERR_NAME_TAKEN){
            fprintf(stderr, "Name is already taken. Choose a different name.\n");
            fflush(stderr);
            free(response);
            response = NULL;
        }else if(response->type == RESPONSE_ERR && response->status == STATUS_ERR_SERVER_ERROR){
            fprintf(stderr, "Internal server error!\n");
            fflush(stderr);
            free(response);
            close(server_socket);
            exit(0);
        }else{
            fprintf(stderr, "Invalid response from server!\n");
            fflush(stderr);
            free(response);
            close(server_socket);
            exit(0);
        }
    }

    // Setup client context and spawn threads
    ClientContext *client_context = (ClientContext *)malloc(sizeof(*client_context));
    if(client_context == NULL)
        exit_with_error("Unable to setup client context!");

    init_client_context(client_context, server_socket);
    pthread_create(&client_context->input_thread, NULL, input_thread_function, NULL);
    pthread_create(&client_context->receiver_thread, NULL, receiver_thread_function, NULL);

    pthread_join(client_context->input_thread, NULL);
    pthread_join(client_context->receiver_thread, NULL);
    
    destroy_client_context(client_context);
    free(client_context);
    close(server_socket);
    return 0;
}