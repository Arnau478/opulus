#include "vm.h"
#include "chunk.h"

VM vm;

void initVM(){
    
}

void freeVM(){

}

static InterpretResult run(){
#define READ_CONSTANT() (*vm.ip++)
#define READ_BYTE() (*vm.ip++)

    for(;;){
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
