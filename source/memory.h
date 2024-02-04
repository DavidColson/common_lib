// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

#include "types.h"

#include <stdlib.h>

struct NewWrapper {};
inline void* operator new(usize, NewWrapper, void* ptr) {
    return ptr;
}
inline void operator delete(void*, NewWrapper, void*) {}
#define SYS_P_NEW(ptr) new (NewWrapper(), ptr)

#include "memory_tracker.h"
#ifdef MEMORY_TRACKING
#define SYS_ALLOC(size) MallocWrap(size)
#define SYS_REALLOC(ptr, size, oldSize) ReallocWrap(ptr, size, oldSize)
#define SYS_FREE(ptr) FreeWrap(ptr)
#else
#define SYS_ALLOC(size) malloc(size)
#define SYS_REALLOC(ptr, size, oldSize) realloc(ptr, size)
#define SYS_FREE(ptr) free(ptr)
#endif

struct IAllocator {
    virtual void* Allocate(usize size) = 0;
    virtual void* Reallocate(void* ptr, usize size, usize oldSize) = 0;
    virtual void Free(void* ptr) = 0;
};

struct DefaultAllocator : public IAllocator {
    void* Allocate(usize size) override;
    void* Reallocate(void* ptr, usize size, usize oldSize) override;
    void Free(void* ptr) override;
};

static DefaultAllocator g_Allocator;
