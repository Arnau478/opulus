#ifdef DEBUG

#include "debug.h"
#include "value.h"

static int simpleInstruction(const char *name, int offset){
    printf("%s\n", name);
    return offset + 1;
}

static int constantInstruction(const char *name, Chunk *chunk, int offset){
    uint8_t constant = chunk->code[offset + 1];
    printf("%-16s %4d '", name, constant);
    printValue(chunk->constants.values[constant]);
    printf("'\n");
    return offset + 2;
}

int disassembleInstruction(Chunk *chunk, int offset){
    printf("%04d ", offset);
    if(offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]){
        printf("   | ");
    }
    else{
        printf("%4d ", chunk->lines[offset]);
    }

    int instruction = chunk->code[offset];

    switch(instruction){
        case OP_CONSTANT:
            return constantInstruction("OP_CONSTANT", chunk, offset);
        case OP_NEGATE:
            return simpleInstruction("OP_NEGATE", offset);
        case OP_RETURN:
            return simpleInstruction("OP_RETURN", offset);
        default:
            printf("Unknown opcode %d\n", instruction);
    }
}

void disassembleChunk(Chunk *chunk, const char *name){
    printf("== %s ==\n", name);

    printf("%i\n", chunk->count);

    for(int offset = 0; offset < chunk->count;){
        offset = disassembleInstruction(chunk, offset);
    }
}

#endif
