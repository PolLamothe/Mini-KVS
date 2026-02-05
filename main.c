#include "hashage.h"
#include "error.h"
#include <stdlib.h>
#include "input.h"

int main(){
    Error* error = NULL;
    int dataSize = 100;
    char name[] = "test";
    CachedHashMap* hashmap = createHashMap(name,dataSize,50,&error);
    if(error != NULL){
        printErrorStack(error);
        return 1;
    }
    printf("HashMap imported successfully !\n");
    if (hashmap->hashMap->dataSize != dataSize){
        printf("WARNING: taille de hashMap importé différente de celle spécifiée\n");
    }
    while(1){
        char* userAction = getUserInput(name);

        if(strcmp("help",userAction) == 0){
            printf("Insert a value : add TABLE:KEY_INT:VAlUE\n");
        }
        free(userAction);
    }
    return 0;
}