#pragma once

#include <stdint.h>
#include <stddef.h>
#include "memory.h"

typedef enum {
    OP_RETURN,
} OpCode;

typedef struct {
    uint8_t *code;
    int count;
    int capacity;
} Chunk;

void initChunk(Chunk *chunk);
void freeChunk(Chunk *chunk);
void writeChunk(Chunk *chunk, uint8_t byte);
