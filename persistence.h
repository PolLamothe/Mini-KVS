#ifndef PERSISTENCE_H
#define PERSISTENCE_H
#include <stdio.h>
#include <string.h>
#include "error.h"
#include "hashage.h"

FILE* openHashMapFile(char* name);

void createHashMapFile(char* filename,HashMap* hashmap,Error** error);

int* importHashMapFromFile(char* filename,Error** error);

#endif