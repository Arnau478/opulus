#include <math.h>
#include <string.h>
#include <time.h>
#include "vm.h"
#include "chunk.h"
#include "debug.h"
#include "compiler.h"
#include "object.h"

VM vm;

static Value n_clock(int argCount, Value *args){
    return NUMBER_VAL((double)clock() / CLOCKS_PER_SEC);
}

static Value n_input(int argCount, Value *args){
    if(argCount > 0){
        printValue(args[0]);
    }
    char *str = malloc(sizeof(char) * 256);
    scanf("%s", str);
    Value ret = OBJ_VAL(copyString(str, (int)strlen(str)));
    free(str);
    return ret;
}

static Value n_num(int argCount, Value *args){
    if(argCount == 1 && IS_STRING(args[0])){
        return NUMBER_VAL(strtod(AS_CSTRING(args[0]), NULL));
    }
    return NIL_VAL;
}

static Value n_str(int argCount, Value *args){
    if(argCount == 1 && IS_NUMBER(args[0])){
        char *str = malloc(sizeof(char) * 256);
        sprintf(str, "%g", AS_NUMBER(args[0]));
        return OBJ_VAL(copyString(str, (int)strlen(str)));
    }
    return NIL_VAL;
}

static Value n_len(int argCount, Value *args){
    if(argCount == 1){
        if(IS_STRING(args[0])){
            return NUMBER_VAL((double)AS_STRING(args[0])->length);
        }
        else if(IS_ARRAY(args[0])){
            return NUMBER_VAL((double)AS_ARRAY(args[0])->count);
        }
    }
    return NIL_VAL;
}

static Value n_exit(int argCount, Value *args){
    exit(0);
}

static void defineNative(const char *name, NativeFn function){
    push(OBJ_VAL(copyString(name, (int)strlen(name))));
    push(OBJ_VAL(newNative(function)));
    tableSet(&vm.globals, AS_STRING(vm.stack[0]), vm.stack[1]);
    pop();
    pop();
}

static void defineNatives(){
    defineNative("clock", n_clock);
    defineNative("input", n_input);
    defineNative("num", n_num);
    defineNative("str", n_str);
    defineNative("len", n_len);
    defineNative("exit", n_exit);
}

void initVM(){
    resetStack();
    vm.objects = NULL;

    initTable(&vm.globals);
    initTable(&vm.strings);

    defineNatives();
}

void freeVM(){
    freeTable(&vm.globals);
    freeTable(&vm.strings);
    freeObjects();
}

static Value peek(int distance){
    return vm.stackTop[-1 - distance];
}

static void runtimeError(const char *format, ...){
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);
    
    for(int i = vm.frameCount - 1; i >= 0; i--){
        if(vm.frameCount < 8 || i+5 > vm.frameCount || i == 0){
            CallFrame *frame = &vm.frames[i];
            ObjFunction *function = frame->function;
            size_t instruction = frame->ip - function->chunk.code - 1;
            fprintf(stderr, "[line %d] in ", function->chunk.lines[instruction]);

            if(function->name == NULL){
                fprintf(stderr, "script\n");
            }
            else{
                fprintf(stderr, "%s()\n", function->name->chars);
            }
        }
        else if(vm.frameCount < 8 || i+5 == vm.frameCount){
            printf("...\n");
        }
    }

    resetStack();
}

static bool call(ObjFunction *function, int argCount){
    if(argCount != function->arity){
        runtimeError("Expected %d arguments but got %d", function->arity, argCount);
        return false;
    }

    if(vm.frameCount == FRAMES_MAX){
        runtimeError("Stack overflow");
        return false;
    }

    CallFrame *frame = &vm.frames[vm.frameCount++];
    frame->function = function;
    frame->ip = function->chunk.code;
    frame->slots = vm.stackTop - argCount - 1;
    return true;
}

static bool callValue(Value callee, int argCount){
    if(IS_OBJ(callee)){
        switch(OBJ_TYPE(callee)){
            case OBJ_FUNCTION:
                return call(AS_FUNCTION(callee), argCount);
            case OBJ_NATIVE:
                NativeFn native = AS_NATIVE(callee);
                Value result = native(argCount, vm.stackTop - argCount);
                vm.stackTop -= argCount + 1;
                push(result);
                return true;
            default: break;
        }
    }

    runtimeError("Non-callable type");
    return false;
}

static bool isFalsey(Value value){
    return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static void concatenate(){
    ObjString *b = AS_STRING(pop());
    ObjString *a = AS_STRING(pop());

    int length = a->length + b->length;
    char *chars = ALLOCATE(char, length + 1);
    memcpy(chars, a->chars, a->length);
    memcpy(chars + a->length, b->chars, b->length);
    chars[length] = '\0';

    ObjString *result = takeString(chars, length);
    push(OBJ_VAL(result));
}

static void addToArray(ObjArray *array, Value value){
    if(array->count >= array->capacity){
        int oldCapacity = array->capacity;
        array->capacity = GROW_CAPACITY(oldCapacity);
        array->values = GROW_ARRAY(Value, array->values, oldCapacity, array->capacity);
    }

    array->values[array->count] = value;
    array->count++;
}

static void setIndex(ObjArray *array, int index, Value value){
    if(index >= array->count){
        for(int i = 0; i < (index+1)-array->count; i++){
            addToArray(array, NIL_VAL);
        }
        addToArray(array, value);
    }
    else{
        array->values[index] = value;
    }
}

static InterpretResult run(){
    CallFrame *frame = &vm.frames[vm.frameCount - 1];

#define READ_BYTE() (*frame->ip++)
#define READ_SHORT() (frame->ip += 2, (uint16_t)((frame->ip[-2] << 8) | frame->ip[-1]))
#define READ_CONSTANT() (frame->function->chunk.constants.values[READ_BYTE()])
#define READ_STRING() AS_STRING(READ_CONSTANT())
#define BINARY_OP(valueType, op) \
    do{ \
        if(!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))){ \
            runtimeError("Operands must be both numbers"); \
            return INTERPRET_RUNTIME_ERROR; \
        } \
        double b = AS_NUMBER(pop()); \
        double a = AS_NUMBER(pop()); \
        push(valueType(a op b)); \
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
        disassembleInstruction(&frame->function->chunk, (int)(frame->ip - frame->function->chunk.code));
#endif

        uint8_t instruction;
        switch(instruction = READ_BYTE()){
            case OP_CONSTANT:
                Value constant = READ_CONSTANT();
                push(constant);
                break;
            case OP_NIL: push(NIL_VAL); break;
            case OP_TRUE: push(BOOL_VAL(true)); break;
            case OP_FALSE: push(BOOL_VAL(false)); break;
            case OP_POP: pop(); break;
            case OP_GET_LOCAL: {
                uint8_t slot = READ_BYTE();
                push(frame->slots[slot]);
                break;
            }
            case OP_GET_GLOBAL: {
                ObjString *name = READ_STRING();
                Value value;
                if(!tableGet(&vm.globals, name, &value)){
                    runtimeError("Undefined variable '%s'", name->chars);
                    return INTERPRET_RUNTIME_ERROR;
                }
                push(value);
                break;
            }
            case OP_DEFINE_GLOBAL: {
                ObjString *name = READ_STRING();
                tableSet(&vm.globals, name, peek(0));
                pop();
                break;
            }

            case OP_SET_LOCAL: {
                uint8_t slot = READ_BYTE();
                frame->slots[slot] = peek(0);
                break;
            }
            case OP_SET_GLOBAL: {
                ObjString *name = READ_STRING();
                if(tableSet(&vm.globals, name, peek(0))){
                    tableDelete(&vm.globals, name);
                    runtimeError("Undefined variable '%s'", name->chars);
                    return INTERPRET_RUNTIME_ERROR;
                }
                break;
            }
            case OP_EQUAL: {
                Value b = pop();
                Value a = pop();
                push(BOOL_VAL(valuesEqual(a, b)));
                break;
            }
            case OP_NOT_EQUAL: {
                Value b = pop();
                Value a = pop();
                push(BOOL_VAL(!valuesEqual(a, b)));
                break;
            }
            case OP_GREATER:
                BINARY_OP(BOOL_VAL, >);
                break;
            case OP_GREATER_EQUAL:
                BINARY_OP(BOOL_VAL, >=);
                break;
            case OP_LESS:
                BINARY_OP(BOOL_VAL, <);
                break;
            case OP_LESS_EQUAL:
                BINARY_OP(BOOL_VAL, <=);
                break;
            case OP_ADD:
                if(IS_STRING(peek(0)) && IS_STRING(peek(1))){
                    concatenate();
                }
                else if(IS_NUMBER(peek(0)) && IS_NUMBER(peek(1))){
                    double b = AS_NUMBER(pop());
                    double a = AS_NUMBER(pop());
                    push(NUMBER_VAL(a + b));
                }
                else{
                    runtimeError("Operands must be two numbers or two strings");
                    return INTERPRET_RUNTIME_ERROR;
                }
                break;
            case OP_SUBTRACT:
                BINARY_OP(NUMBER_VAL, -);
                break;
            case OP_MULTIPLY:
                BINARY_OP(NUMBER_VAL, *);
                break;
            case OP_DIVIDE:
                BINARY_OP(NUMBER_VAL, /);
                break;
            case OP_NOT:
                push(BOOL_VAL(isFalsey(pop())));
                break;
            case OP_NEGATE:
                if(!IS_NUMBER(peek(0))){
                    runtimeError("Operand must be a number");
                    return INTERPRET_RUNTIME_ERROR;
                }
                push(NUMBER_VAL(-AS_NUMBER(pop())));
                break;
            case OP_PRINT:
                printValue(pop());
                printf("\n");
                break;
            case OP_JUMP: {
                uint16_t offset = READ_SHORT();
                frame->ip += offset;
                break;
            }
            case OP_JUMP_IF_FALSE: {
                uint16_t offset = READ_SHORT();
                if(isFalsey(peek(0))) frame->ip += offset;
                break;
            }
            case OP_LOOP: {
                uint16_t offset = READ_SHORT();
                frame->ip -= offset;
                break;
            }
            case OP_CALL: {
                int argCount = READ_BYTE();
                if(!callValue(peek(argCount), argCount)){
                    return INTERPRET_RUNTIME_ERROR;
                }
                frame = &vm.frames[vm.frameCount - 1];
                break;
            }
            case OP_RETURN:
                Value result = pop();
                vm.frameCount--;
                if(vm.frameCount == 0){
                    pop();
                    return INTERPRET_OK;
                }

                vm.stackTop = frame->slots;
                push(result);
                frame = &vm.frames[vm.frameCount - 1];
                break;
            case OP_SPAWN_ARRAY: {
                int elementCount = READ_BYTE();
                ObjArray *array = newArray();
                for(int i = 0; i < elementCount; i++){
                    addToArray(array, *(vm.stackTop-(elementCount-i)));
                }
                vm.stackTop -= elementCount;
                push(OBJ_VAL(array));
                break;
            }
            case OP_INDEX: {
                if(!IS_ARRAY(peek(1))){
                    runtimeError("Only arrays are indexable");
                    return INTERPRET_RUNTIME_ERROR;
                }
                if(!IS_NUMBER(peek(0))){
                    runtimeError("Index must be a number");
                    return INTERPRET_RUNTIME_ERROR;
                }

                double dIndex = AS_NUMBER(pop());
                if(ceilf(dIndex) != dIndex){
                    runtimeError("Index must be integer");
                    return INTERPRET_RUNTIME_ERROR;
                }
                int index = (int)dIndex;

                ObjArray *array = AS_ARRAY(pop());

                if(index >= array->count || index < 0){
                    runtimeError("Index %i out of bounds", index);
                    return INTERPRET_RUNTIME_ERROR;
                }

                push(array->values[index]);
                break;
            }
            case OP_WRITE_INDEX: {
                if(!IS_ARRAY(peek(2))){
                    runtimeError("Only arrays are indexable");
                    return INTERPRET_RUNTIME_ERROR;
                }
                if(!IS_NUMBER(peek(1))){
                    runtimeError("Index must be a number");
                    return INTERPRET_RUNTIME_ERROR;
                }

                Value value = pop();

                double dIndex = AS_NUMBER(pop());
                if(ceilf(dIndex) != dIndex){
                    runtimeError("Index must be an integer");
                    return INTERPRET_RUNTIME_ERROR;
                }
                int index = (int)dIndex;

                ObjArray *array = AS_ARRAY(peek(0));

                setIndex(array, index, value);
                break;
            }
        }
    }

#undef BINARY_OP
#undef READ_STRING
#undef READ_CONSTANT
#undef READ_SHORT
#undef READ_BYTE
}

InterpretResult interpret(const char *source){
    ObjFunction *function = compile(source);
    if(function == NULL) return INTERPRET_COMPILE_ERROR;

    push(OBJ_VAL(function));
    call(function, 0);

    return run();
}

void resetStack(){
    vm.stackTop = vm.stack;
    vm.frameCount = 0;
}

void push(Value value){
    *vm.stackTop = value;
    vm.stackTop++;
}

Value pop(){
    vm.stackTop--;
    return *vm.stackTop;
}
