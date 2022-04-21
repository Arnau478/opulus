#include <string.h>
#include "object.h"
#include "memory.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, objectType) \
    (type *)allocateObject(sizeof(type), objectType);

static Obj *allocateObject(size_t size, ObjType type){
    Obj *object = (Obj *)reallocate(NULL, 0, size);
    object->type = type;

    object->next = vm.objects;
    vm.objects = object;

    return object;
}

static ObjString *allocateString(char *chars, int length, uint32_t hash){
    ObjString *string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
    string->length = length;
    string->chars = chars;
    string->hash = hash;
    tableSet(&vm.strings, string, NIL_VAL);
    return string;
}

static uint32_t hashString(const char *key, int length){
    uint32_t hash = 2166136261u;
    for(int i = 0; i < length; i++){
        hash ^= (uint8_t)key[i];
        hash *= 16777619;
    }
    return hash;
}

ObjFunction *newFunction(){
    ObjFunction *function = ALLOCATE_OBJ(ObjFunction, OBJ_FUNCTION);
    function->arity = 0;
    function->name = NULL;
    initChunk(&function->chunk);
    return function;
}

ObjNative *newNative(NativeFn function){
    ObjNative *native = ALLOCATE_OBJ(ObjNative, OBJ_NATIVE);
    native->function = function;
    return native;
}

ObjString *takeString(char *chars, int length){
    uint32_t hash = hashString(chars, length);
    ObjString *interned = tableFindString(&vm.strings, chars, length, hash);
    if(interned != NULL){
        FREE_ARRAY(char, chars, length + 1);
        return interned;
    }

    return allocateString(chars, length, hash);
}

ObjString *copyString(const char *chars, int length){
    uint32_t hash = hashString(chars, length);
    ObjString *interned = tableFindString(&vm.strings, chars, length, hash);
    if(interned != NULL) return interned;
    char *heapChars = ALLOCATE(char, length + 1);
    memcpy(heapChars, chars, length);
    heapChars[length] = '\0';
    return allocateString(heapChars, length, hash);
}

static void printFunction(ObjFunction *function){
    if(function->name == NULL){
        printf("<script>");
        return;
    }
    printf("<fn %s>", function->name->chars);
}

ObjArray *newArray(){
    ObjArray *array = ALLOCATE_OBJ(ObjArray, OBJ_ARRAY);
    array->values = NULL;
    array->capacity = 0;
    array->count = 0;
    return array;
}

void printObject(Value value){
    switch(OBJ_TYPE(value)){
        case OBJ_FUNCTION:
            printFunction(AS_FUNCTION(value));
            break;
        case OBJ_NATIVE:
            printf("<native fn>");
            break;
        case OBJ_STRING:
            printf("%s", AS_CSTRING(value));
            break;
        case OBJ_ARRAY:
            printf("[");
            bool first = true;
            ObjArray *array = AS_ARRAY(value);
            for(int i = 0; i < array->count; i++){
                if(i != 0) printf(", ");
                printValue(array->values[i]);
            }
            printf("]");
            break;
        default:
            printf("<OBJ>");
            break;
    }
}