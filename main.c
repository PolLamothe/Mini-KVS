#include "hashage.h"
#include "error.h"
#include <stdlib.h>
#include "input.h"
#include "persistence.h"
#include "hashage.h"

int main(){
    Error* error = NULL;
    int dataSize = 100;
    char name[] = "test.db";

    CachedHashMap* hashmap = createHashMap(name,dataSize,50,&error);
    if(error != NULL){
        printErrorStack(error);
        return -1;
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
            return -1;
        }
        if(userAction->type == INSERT){
            Error* insertionError = NULL;
            insertEntry(name,userAction->entry,&insertionError);
            if(insertionError != NULL){
                printErrorStack(insertionError);
                return -1;
            }
            Error* addEntryInCachedHashMapError = NULL;
            addEntryInCachedHashMap(hashmap,userAction->entry,&addEntryInCachedHashMapError);
            if(addEntryInCachedHashMapError != NULL){
                printErrorStack(addEntryInCachedHashMapError);
                return -1;
            }

            printEntry(userAction->entry);
            printf("Inserted successfully !\n");
        }else if(userAction->type == FIND){
            Error* searchEntryInCachedHashMapError = NULL;
            CachedEntry* result = searchEntryInCachedHashMap(hashmap,userAction->entry->table,userAction->entry->id,&searchEntryInCachedHashMapError);
            if(searchEntryInCachedHashMapError != NULL){
                printErrorStack(searchEntryInCachedHashMapError);
                return -1;
            }
            if(result != NULL){
                printf("Data found in the cache !\n");
                printEntry(result->entry);                
            }else{
                printf("Data not found in the cache\n");
                Error* searchEntryInFileError = NULL;
                Entry* fileResult = searchEntryInFile(name,userAction->entry->table,userAction->entry->id,&searchEntryInFileError);
                if(searchEntryInFileError != NULL){
                    printErrorStack(searchEntryInCachedHashMapError);
                    return -1;
                }
                if(fileResult == NULL){
                    printf("Data not found in the db file\n");
                }else{
                    printf("Data found in the db file !\n");
                    printEntry(fileResult);
                }
            }
        }
        free(userInput);
    }
    return 0;
}