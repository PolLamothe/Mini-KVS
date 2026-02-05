#ifndef ERROR_H
#define ERROR_H
#include <stdbool.h>
#include "string.h"

typedef struct Error{
    char* function;
    char* reason;
    char* debug;
    struct Error* stack;
}Error;

void printErrorStack(Error* error);

void printError(Error* error);

void createError(Error** error,char* function,char* reason,char* debug,Error* stack);

void freeError(Error* error);

#endif