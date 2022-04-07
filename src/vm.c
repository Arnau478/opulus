#include "vm.h"
#include "chunk.h"

VM vm;

void initVM(){
    
}

void freeVM(){

}

static InterpretResult run(){
#define READ_BYTE() (*vm.ip++)

    for(;;){
        uint8_t instruction;
        switch(instruction = READ_BYTE()){
            case OP_RETURN:
                return INTERPRET_OK;
        }
    }

#undef READ_BYTE
}

InterpretResult interpret(Chunk *chunk){
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;
    return run();
}
