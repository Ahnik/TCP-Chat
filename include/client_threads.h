#ifndef CLIENT_THREADS_H
#define CLIENT_THREADS_H

// Input thread function. Responsible for taking input from the client and sending them to the server.
void *input_thread_function(void *arg);

// Output thread function. Responsible for receiving server responses and displaying them on the terminal.
void *output_thread_function(void *arg);

#endif