#ifdef DEBUG

#include "debug.h"

static int simpleInstruction(const char *name, int offset){
    printf("%s\n", name);
    return offset + 1;
}

int disassembleInstruction(Chunk *chunk, int offset){
    printf("%04d ", offset);

    int instruction = chunk->code[offset];

    switch(instruction){
        case OP_RETURN:
            return simpleInstruction("OP_RETURN", offset);
        default:
            printf("Unknown opcode %d\n", instruction);
    }
}

void disassembleChunk(Chunk *chunk, const char *name){
    printf("== %s ==\n", name);

    for(int offset = 0; offset < chunk->count;){
        offset += disassembleInstruction(chunk, offset);
    }
}

#endif
