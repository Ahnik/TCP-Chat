#include "input.h"
#include "client_context.h"
#include "common.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

void *input_thread_function(void *arg){
    bool quit = false;
    while(!quit){
        char input_buffer[MAX_MESSAGE_SIZE];
        if(fgets(input_buffer, MAX_MESSAGE_SIZE, stdin) != NULL){
            input_buffer[strcspn(input_buffer, "\n")] = '\0';

        }
    }
}