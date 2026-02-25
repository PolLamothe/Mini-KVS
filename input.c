#include <stdlib.h>
#include <stdio.h>
#include "input.h"
#include "error.h"
#include <ctype.h>
#include <stdbool.h>

char* getUserInput(char* name){
    char userAction[255];
    printf("%s: ",name);
    fgets(userAction, sizeof(userAction), stdin);
    userAction[strcspn(userAction, "\r\n")] = '\0';
    char* result = malloc(strlen(userAction)+1);
    strcpy(result,userAction);
    return result;
}

void freeUserAction(UserAction* userAction,Error** error){
    char* functionName = "input.freeUserAction";
    if (*error != NULL){
        createError(error,functionName,"Error must be null",NULL,NULL);
        return;
    }  
    if(userAction->entry != NULL){
        Error* freeEntryError = NULL;
        freeEntry(userAction->entry,&freeEntryError);
        if(freeEntryError != NULL){
            createError(error,functionName,"Error during free Entry",NULL,freeEntryError);
        return;
        }
    }
    free(userAction);
}

UserAction* getEntry(char* input,Error** error){
    char* functionName = "input.getEntry";
    if (*error != NULL){
        createError(error,functionName,"Error must be null",NULL,NULL);
        return NULL;
    } 
    
    int actionIndex = -1;
    for(int i = 0;i<strlen(input);i++){
        if(input[i] == ':'){
            actionIndex = i;
            break;
        }else{
            input[i] = toupper(input[i]);
        }
    }
    if(actionIndex == -1){
        printf("%d\n",(int)strlen(input));
        createError(error,functionName,"No delimiter found for action",NULL,NULL);
        return NULL;
    }

    char actionString[actionIndex+1];
    
    strncpy(actionString, input, actionIndex);

    ActionType resultType;

    if(strcmp(actionString,"ADD") == 0){
        resultType = INSERT;
    }else if(strcmp(actionString,"FIND") == 0){
        resultType = FIND;
    }else{
        createError(error,functionName,"Invalid action",actionString,NULL);
        return NULL;
    }

    int tableStartIndex = -1;
    int tableEndIndex = -1;
    for(int i = actionIndex;i<strlen(input);i++){
        if(tableStartIndex == -1){
            if(input[i] != ':'){
                tableStartIndex = i;
            }
        }else if(tableEndIndex == -1){
            if(input[i] == ':'){
                tableEndIndex = i;
                break;
            }
        }
    }
    if(tableStartIndex == -1){
        createError(error,functionName,"No table found",NULL,NULL);
        return NULL;
    }
    if(tableEndIndex == -1){
        createError(error,functionName,"No delimiter found for Table",NULL,NULL);
        return NULL;
    }
    char table[(tableEndIndex-tableStartIndex+1)];
    table[(tableEndIndex-tableStartIndex)] = '\0';
    strncpy(table, input+tableStartIndex, tableEndIndex-tableStartIndex);

    int keyStartIndex = -1;
    int keyEndIndex = -1;
    for(int i = tableEndIndex+1;i<strlen(input);i++){
        if(keyStartIndex == -1){
            if(input[i] != ':'){
                keyStartIndex = i;
            }
        }else{
            if(input[i] == ':'){
                keyEndIndex = i;
                break;
            }else{
                if(!isdigit(input[i])){
                    createError(error,functionName,"Key must be an int",NULL,NULL);
                    return NULL;
                }
            }
        }
    }
    if(keyStartIndex == -1){
        createError(error,functionName,"No Key found",NULL,NULL);
        return NULL;
    }
    if(keyEndIndex == -1 && resultType == INSERT){
        createError(error,functionName,"No delimiter found for Key",NULL,NULL);
        return NULL;
    }
    char* keyChar;

    if(keyEndIndex != -1){
        //If the key isn't the end of the input
        keyChar = malloc((keyEndIndex-keyStartIndex+1)*sizeof(char));
        keyChar[(keyEndIndex-keyStartIndex)] = '\0';
        strncpy(keyChar, input+keyStartIndex, keyEndIndex-keyStartIndex);
    }else{
        keyChar = malloc((strlen(input)-keyStartIndex+1)*sizeof(char));
        keyChar[(strlen(input)-keyStartIndex)] = '\0';
        strncpy(keyChar, input+keyStartIndex, strlen(input)-keyStartIndex);
    }

    int key = atoi(keyChar);

    if(resultType == FIND){
        Error* createEntryError = NULL;
        Entry* resultEntry = createEntry(table,key,NULL,STRING,NULL,NULL,&createEntryError);
        if(createEntryError != NULL){
            createError(error,functionName,"Error during the creation of the entry",NULL,createEntryError);
            free(keyChar);
            return NULL;    
        }

        UserAction* result = malloc(sizeof(UserAction));
        *result = (UserAction){
            .type = resultType,
            .entry = resultEntry
        };
        free(keyChar);

        return result;
    }

    int valueStartIndex = -1;
    int valueEndIndex = strlen(input);
    bool isInt = true;
    for(int i = keyEndIndex+1;i<strlen(input);i++){
        if(!isdigit(input[i])){
            isInt = false;
        }
        if(valueStartIndex == -1){
            if(input[i] != ':'){
                valueStartIndex = i;
            }
        }
    }
    if(valueStartIndex == -1){
        createError(error,functionName,"No Value found",NULL,NULL);
        free(keyChar);
        return NULL;
    }

    char* value[(strlen(input)-valueStartIndex+1)];
    value[strlen(input)-valueStartIndex] = '\0';
    strncpy(value, input+valueStartIndex, strlen(input)-valueStartIndex);

    void* resultValue;
    EntryValueType resultValueType;

    if(isInt){
        int32_t valueInt = (int32_t)value;
        resultValue = &valueInt;
        resultValueType = INT32;
    }else{
        resultValue = value;
        resultValueType = STRING;
    }

    Error* createEntryError = NULL;
    Entry* resultEntry = createEntry(table,key,resultValue,resultValueType,NULL,NULL,&createEntryError);
    if(createEntryError != NULL){
        createError(error,functionName,"Error during the creation of the entry",NULL,createEntryError);
        free(keyChar);
        return NULL;    
    }

    UserAction* result = malloc(sizeof(UserAction));
    *result = (UserAction){
        .type = resultType,
        .entry = resultEntry
    };

    free(keyChar);
    return result;
}

void printUserAction(UserAction* userAction){
    if(userAction->type == INSERT){
        printf("Action : INSERT\n");
    }else{
        printf("Unkown Action\n");
    }
    printEntry(userAction->entry);
}