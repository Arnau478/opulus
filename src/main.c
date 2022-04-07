#include "chunk.h"
#include "debug.h"
#include "vm.h"

int main(int argc, char **argv){
    initVM();

    Chunk chunk;
    initChunk(&chunk);

    writeChunk(&chunk, OP_RETURN);

#ifdef DEBUG
    disassembleChunk(&chunk, "test chunk");
#endif
    interpret(&chunk);

    freeVM();
    freeChunk(&chunk);
}
