#include "chunk.h"
#include "debug.h"
#include "vm.h"

int main(int argc, char **argv){
    initVM();

    Chunk chunk;
    initChunk(&chunk);

    int constant = addConstant(&chunk, 12.34);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant, 123);

    writeChunk(&chunk, OP_NEGATE, 123);

    writeChunk(&chunk, OP_RETURN, 123);

#ifdef DEBUG
    disassembleChunk(&chunk, "test chunk");
#endif
    interpret(&chunk);

    freeVM();
    freeChunk(&chunk);
}
