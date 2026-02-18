#include <stdlib.h>
#include <stdio.h>
#include "input.h"
#include "error.h"
#include <ctype.h>
#include <stdbool.h>

char* getUserInput(char* name){
    char* userAction = malloc(sizeof(char)*255);
    printf("%s: ",name);
    scanf("%254s",userAction);
    return userAction;
}

UserAction* getEntry(char* input,Error** error){
    char* functionName = "input.getEntry";
    if (*error != NULL){
        createError(error,functionName,"Error must be null",NULL,NULL);
        return NULL;
    }    

    UserAction* result = (UserAction*)malloc(sizeof(UserAction));

    result->entry = (Entry*)malloc(sizeof(Entry));
    
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

    if(strcmp(actionString,"ADD") == 0){
        result->type = INSERT;
    }else if(strcmp(actionString,"FIND") == 0){
        result->type = FIND;
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
    char* table = malloc((tableEndIndex-tableStartIndex+1)*sizeof(char));
    table[(tableEndIndex-tableStartIndex)] = '\0';
    strncpy(table, input+tableStartIndex, tableEndIndex-tableStartIndex);
    result->entry->table = table;

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
    if(keyEndIndex == -1 && result->type == INSERT){
        createError(error,functionName,"No delimiter found for Key",NULL,NULL);
        return NULL;
    }
    char* keyChar;
    if(keyEndIndex != -1){
        keyChar = malloc((keyEndIndex-keyStartIndex+1)*sizeof(char));
        keyChar[(keyEndIndex-keyStartIndex)] = '\0';
        strncpy(keyChar, input+keyStartIndex, keyEndIndex-keyStartIndex);
    }else{
        keyChar = malloc((strlen(input)-keyStartIndex+1)*sizeof(char));
        keyChar[(strlen(input)-keyStartIndex)] = '\0';
        strncpy(keyChar, input+keyStartIndex, strlen(input)-keyStartIndex);
    }
    int key = atoi(keyChar);
    result->entry->id = key;

    if(result->type == FIND){
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
        return NULL;
    }

    char* value = malloc((strlen(input)-valueStartIndex+1)*sizeof(char));
    value[strlen(input)-valueStartIndex] = '\0';
    strncpy(value, input+valueStartIndex, strlen(input)-valueStartIndex);

    if(isInt){
        int* valueAdress = malloc(sizeof(int));
        *valueAdress = (int)value;
        result->entry->value = valueAdress;
        result->entry->valueSize = sizeof(int);
    }else{
        result->entry->value = value;
        result->entry->valueSize = sizeof(char) * strlen(value);
    }
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