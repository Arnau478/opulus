#include "memory.h"
#include "object.h"
#include "vm.h"

void *reallocate(void *ptr, size_t oldSize, size_t newSize){
    if(newSize == 0){
        free(ptr);
        return NULL;
    }

    void *res = realloc(ptr, newSize);
    if(res == NULL) exit(99);
    return res;
}

static void freeObject(Obj *object){
    switch(object->type){
        case OBJ_STRING: {
            ObjString *string = (ObjString *)object;
            FREE_ARRAY(char, string->chars, string->length + 1);
            FREE(ObjString, object);
            break;
        }
    }
}

void freeObjects(){
    Obj *object = vm.objects;
    while(object != NULL){
        Obj *next = object->next;
        freeObject(object);
        object = next;
    }
}
