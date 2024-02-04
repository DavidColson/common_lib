// Copyright 2020-2022 David Colson. All rights reserved.

#include "memory.h"

// ***********************************************************************

void* DefaultAllocator::Allocate(usize size) {
    return SYS_ALLOC(size);
}

// ***********************************************************************

void* DefaultAllocator::Reallocate(void* ptr, usize size, usize oldSize) {
    return SYS_REALLOC(ptr, size, oldSize);
}

// ***********************************************************************

void DefaultAllocator::Free(void* ptr) {
    SYS_FREE(ptr);
}
