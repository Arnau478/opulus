#include "chunk.h"
#include "debug.h"
#include "vm.h"

int main(int argc, char **argv){
    initVM();

    Chunk chunk;
    initChunk(&chunk);

    int constant = addConstant(&chunk, 12.34);
    writeChunk(&chunk, OP_CONSTANT);
    writeChunk(&chunk, constant);

    writeChunk(&chunk, OP_RETURN);

#ifdef DEBUG
    disassembleChunk(&chunk, "test chunk");
#endif
    interpret(&chunk);

    freeVM();
    freeChunk(&chunk);
}
