#include "vm.h"
#include "chunk.h"
#include "debug.h"
#include "compiler.h"

VM vm;

void initVM(){
    resetStack();
}

void freeVM(){

}

static InterpretResult run(){
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(op) \
    do{ \
        double b = pop(); \
        double a = pop(); \
        push(a op b); \
    } \
    while(0)

#ifdef DEBUG
        printf("== execution trace ==\n");
#endif

    for(;;){
#ifdef DEBUG
        printf("          ");
        for(Value *slot = vm.stack; slot < vm.stackTop; slot++){
            printf("[ ");
            printValue(*slot);
            printf(" ]");
        }
        printf("\n");
        disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif

        uint8_t instruction;
        switch(instruction = READ_BYTE()){
            case OP_CONSTANT:
                Value constant = READ_CONSTANT();
                push(constant);
                break;
            case OP_ADD:
                BINARY_OP(+);
                break;
            case OP_SUBTRACT:
                BINARY_OP(-);
                break;
            case OP_MULTIPLY:
                BINARY_OP(*);
                break;
            case OP_DIVIDE:
                BINARY_OP(/);
                break;
            case OP_NEGATE:
                vm.stackTop[-1] *= -1;
                break;
            case OP_RETURN:
                printValue(pop());
                printf("\n");
                return INTERPRET_OK;
        }
    }

#undef BINARY_OP
#undef READ_CONSTANT
#undef READ_BYTE
}

InterpretResult interpret(const char *source){
    compile(source);
    return INTERPRET_OK;
}

void resetStack(){
    vm.stackTop = vm.stack;
}

void push(Value value){
    *vm.stackTop = value;
    vm.stackTop++;
}

Value pop(){
    vm.stackTop--;
    return *vm.stackTop;
}
