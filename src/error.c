#include "error.h"

const char *error_to_string(int error){
    switch(error){
        case ERR_OK:                   return "No error";
        case ERR_MEMORY:               return "Memory allocation failed";
        case ERR_INVALID:              return "The action was invalid";
        case ERR_BUFFER_SIZE_EXCEEDED: return "Buffer size exceeded";
        case ERR_RECEIVING:            return "Receiving error";
        default:                       return "Unknown error";
    }
}