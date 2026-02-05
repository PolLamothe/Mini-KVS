#include <stdio.h>
#include <stdlib.h>
#include "error.h"
#include <stdbool.h>

void printErrorStack(Error* error){
    Error* currentError = error;
    while(currentError != NULL){
        printError(currentError);
        printf("\n");
        currentError = currentError->stack;
    }
}

void printError(Error* error){
    printf("function : %s\nreason : %s\ndebug : %s\n",error->function,error->reason,error->debug);
}

void createError(Error** error,char* function,char* reason,char* debug,Error* stack){
    *error = malloc(sizeof(Error));
    **error = (Error){
        .function = function,
        .reason = reason,
        .stack = stack,
        .debug = debug
    };
};

void freeError(Error* error){
    Error* current = error;
    while(current != NULL){
        Error* save = current;
        current = current->stack;
        free(save);
    }
}