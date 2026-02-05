#include <stdlib.h>
#include <stdio.h>
#include "input.h"
#include "error.h"

char* getUserInput(char* name){
    char* userAction = malloc(sizeof(char)*255);
    printf("%s: ",name);
    scanf("%254s",userAction);
    return userAction;
}