#include "hashage.h"
#include "error.h"
#include <stdlib.h>
#include "input.h"
#include "persistence.h"

int main(){
    Error* error = NULL;
    int dataSize = 100;
    char name[] = "test.db";

    CachedHashMap* hashmap = createHashMap(name,dataSize,50,&error);
    if(error != NULL){
        printErrorStack(error);
        return 1;
    }
    printf("HashMap imported successfully !\n");
    if (hashmap->hashMap->dataSize != dataSize){
        printf("WARNING: HashMap size different from the one specified\n");
    }
    while(1){
        char* userInput = getUserInput(name);

        if(strcmp("help",userInput) == 0){
            printf("Insert a value : add:TABLE:KEY_INT:VAlUE\n");
            free(userInput);
            continue;
        }

        Error* userActionError = NULL;
        UserAction* userAction = getEntry(userInput,&userActionError);
        if(userActionError != NULL){
            printErrorStack(userActionError);
            return 1;
        }
        printUserAction(userAction);
        free(userInput);
    }
    return 0;
}