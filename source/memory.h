#pragma once

#include <stdlib.h>

struct NewWrapper {};
inline void* operator new(size_t, NewWrapper, void* ptr) {
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
    virtual void* Allocate(size_t size) = 0;
    virtual void* Reallocate(void* ptr, size_t size, size_t oldSize) = 0;
    virtual void Free(void* ptr) = 0;
};

struct DefaultAllocator : public IAllocator {
    void* Allocate(size_t size) override;
    void* Reallocate(void* ptr, size_t size, size_t oldSize) override;
    void Free(void* ptr) override;
};

static DefaultAllocator gAllocator;
