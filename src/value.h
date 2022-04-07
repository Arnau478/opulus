#pragma once

#include <stddef.h>
#include <stdio.h>
#include "memory.h"

typedef double Value;

typedef struct {
    Value *values;
    int count;
    int capacity;
} ValueArray;

void initValueArray(ValueArray *array);
void freeValueArray(ValueArray *array);
void writeValueArray(ValueArray *array, Value value);

void printValue(Value value);
