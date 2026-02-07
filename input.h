#ifndef INPUT_H
#define INPUT_H
#include <stdbool.h>
#include "error.h"
#include "hashage.h"

enum ActionType {INSERT, FIND};

typedef struct UserAction{
    enum ActionType type;
    Entry* entry;
} UserAction;

char* getUserInput(char* name);

UserAction* getEntry(char* input,Error** error);

void printUserAction(UserAction* userAction);

#endif