#pragma once

#define STACK_MAX 256

#include <stdarg.h>
#include <string.h>
#include "chunk.h"
#include "compiler.h"
#include "debug.h"
#include "object.h"
#include "table.h"

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
    Table globals;
    Table strings;
    Obj *objects;
} VM;

extern VM vm;

void initVM();
void freeVM();

InterpretResult interpret(const char *source);

void resetStack();
void push(Value value);
Value pop();
