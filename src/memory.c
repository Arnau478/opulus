#include "memory.h"

void *reallocate(void *ptr, size_t oldSize, size_t newSize){
    if(newSize == 0){
        free(ptr);
        return NULL;
    }

    void *res = realloc(ptr, newSize);
    if(res == NULL) exit(99);
    return res;
}
