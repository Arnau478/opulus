#include "chunk.h"

#include "debug.h"

int main(int argc, char **argv){
    Chunk chunk;
    initChunk(&chunk);

    writeChunk(&chunk, OP_RETURN);

#ifdef DEBUG
    disassembleChunk(&chunk, "test chunk");
#endif

    freeChunk(&chunk);
}
