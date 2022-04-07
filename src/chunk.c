#include "chunk.h"

void initChunk(Chunk *chunk){
    chunk->code = NULL;
    chunk->count = 0;
    chunk->capacity = 0;
}

void freeChunk(Chunk *chunk){
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    initChunk(chunk);
}

void writeChunk(Chunk *chunk, uint8_t byte){
    if(chunk->count >= chunk->capacity){
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
    }

    chunk->code[chunk->count++] = byte;
}
