#pragma once

#include <stdlib.h>
#include <stddef.h>

#define ALLOCATE(type, count) \
    (type *)reallocate(NULL, 0, sizeof(type) * (count))

#define FREE(type, ptr) reallocate(ptr, sizeof(type), 0);

#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

#define GROW_ARRAY(type, ptr, oldCount, newCount) \
    (type*)reallocate(ptr, sizeof(type) * (oldCount), sizeof(type) * (newCount))

#define FREE_ARRAY(type, ptr, count) \
    reallocate(ptr, sizeof(type) * (count), 0)

void *reallocate(void *ptr, size_t oldSize, size_t newSize);
void freeObjects();
