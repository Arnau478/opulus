#include "vm.h"
#include "chunk.h"
#include "debug.h"

VM vm;

void initVM(){
    
}

void freeVM(){

}

static InterpretResult run(){
#define READ_CONSTANT() (*vm.ip++)
#define READ_BYTE() (*vm.ip++)

#ifdef DEBUG
        printf("== execution trace ==\n");
#endif

    for(;;){
#ifdef DEBUG
        disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif

        uint8_t instruction;
        switch(instruction = READ_BYTE()){
            case OP_CONSTANT:
                Value constant = READ_CONSTANT();
                break;
            case OP_RETURN:
                return INTERPRET_OK;
        }
    }

#undef READ_BYTE
#undef READ_CONSTANT
}

InterpretResult interpret(Chunk *chunk){
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;
    return run();
}
