#include <string.h>
#include <time.h>
#include "natives.h"
#include "vm.h"

static void defineNative(const char *name, NativeFn function){
    push(OBJ_VAL(copyString(name, (int)strlen(name))));
    push(OBJ_VAL(newNative(function)));
    tableSet(&vm.globals, AS_STRING(vm.stack[0]), vm.stack[1]);
    pop();
    pop();
}

// NATIVES

static Value n_clock(int argCount, Value *args){
    return NUMBER_VAL((double)clock() / CLOCKS_PER_SEC);
}



void defineNatives(){
    defineNative("clock", n_clock);
}
