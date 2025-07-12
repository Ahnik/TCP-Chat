#include "common.h"
#include "protocol.h"
#include "message.h"
#include <sys/socket.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>

int main(int argc, char **argv){
    // Usage check
    if(argc != 2) exit_with_error("Usage: %s <server address>", argv[0]);

    // Create the client socket
    int client_socket;
    if((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
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
    if(connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        exit_with_error("Connection failed!");

    // Variable to store username
    char username[MAX_USERNAME_SIZE];

    // Clear the screen and take username as input
    printf("\033[H\033[J");
    printf("Enter your name: ");
    if(fgets(username, MAX_USERNAME_SIZE, stdin) == NULL)
        exit_with_error("Input name error!");

    // Send JOIN request to the server
    Request *request = (Request *)malloc(sizeof(*request));
    if(request == NULL)
        exit_with_error("Memory allocation for request failed!");
    
    snprintf(request->username, MAX_USERNAME_SIZE, "%s", username);
    request->type = REQUEST_JOIN;
    uint32_t length = request_length(request);


    close(client_socket);
    return 0;
}