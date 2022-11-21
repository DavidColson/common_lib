#pragma once

#include <stdlib.h>

struct NewWrapper {};
inline void* operator new(size_t, NewWrapper, void* ptr) { return ptr; }
inline void operator delete(void*, NewWrapper, void*) {}

#define SYS_ALLOC(size) malloc(size)
#define SYS_REALLOC(ptr, size) realloc(ptr, size)
#define SYS_FREE(ptr) free(ptr)
#define SYS_P_NEW(ptr) new(NewWrapper(), ptr)

