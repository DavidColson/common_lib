#pragma once

#include <stdlib.h>

struct NewWrapper {};
inline void* operator new(size_t, NewWrapper, void* ptr) { return ptr; }
inline void operator delete(void*, NewWrapper, void*) {}
#define SYS_P_NEW(ptr) new(NewWrapper(), ptr)

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

struct Allocator {
	// begin allocator required interface
	Allocator();
	Allocator(const char* _name);

	void* 	Allocate(size_t size);
	void* 	Reallocate(void* ptr, size_t size, size_t oldSize);
	void 	Free(void* ptr);

	const char* GetName();
	void 		SetName(const char* _name);
	// end allocator required interface

private:
	const char* name;
};