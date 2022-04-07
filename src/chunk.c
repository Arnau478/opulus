#include "chunk.h"

void initChunk(Chunk *chunk){
    chunk->code = NULL;
    chunk->count = 0;
    chunk->capacity = 0;
    initValueArray(&chunk->constants);
}

void freeChunk(Chunk *chunk){
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    freeValueArray(&chunk->constants);
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

int addConstant(Chunk *chunk, Value value){
    writeValueArray(&chunk->constants, value);
    return chunk->constants.count - 1;
}
