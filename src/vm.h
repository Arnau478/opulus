#pragma once

#define STACK_MAX 256

#include <stdarg.h>
#include "chunk.h"

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} InterpretResult;

typedef struct {
    Chunk *chunk;
    uint8_t *ip;
    Value stack[STACK_MAX];
    Value *stackTop;
    Obj *objects;
} VM;

extern VM vm;

void initVM();
void freeVM();

InterpretResult interpret(const char *source);

void resetStack();
void push(Value value);
Value pop();
