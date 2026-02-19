#ifndef INPUT_H
#define INPUT_H
#include <stdbool.h>
#include "error.h"
#include "hashage.h"

typedef enum ActionType {INSERT, FIND} ActionType;

typedef struct UserAction{
    ActionType type;
    Entry* entry;
} UserAction;

void freeUserAction(UserAction* userAction,Error** error);

char* getUserInput(char* name);

UserAction* getEntry(char* input,Error** error);

void printUserAction(UserAction* userAction);

#endif