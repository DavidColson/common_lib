#pragma once

#include <stdlib.h>

struct NewWrapper {};
inline void* operator new(size_t, NewWrapper, void* ptr) { return ptr; }
inline void operator delete(void*, NewWrapper, void*) {}
#define SYS_P_NEW(ptr) new(NewWrapper(), ptr)

#ifdef MEMORY_TRACKING
#include "memory_tracker.h"
#define SYS_ALLOC(size) MallocTrack(size)
#define SYS_REALLOC(ptr, size) ReallocTrack(ptr, size)
#define SYS_FREE(ptr) FreeTrack(ptr)
#else
#define SYS_ALLOC(size) malloc(size)
#define SYS_REALLOC(ptr, size) realloc(ptr, size)
#define SYS_FREE(ptr) free(ptr)
#endif

struct Allocator {
	// begin allocator required interface
	Allocator();
	Allocator(const char* _name);

	void* 	Allocate(size_t size);
	void* 	Reallocate(void* ptr, size_t size);
	void 	Free(void* ptr);

	const char* GetName();
	void 		SetName(const char* _name);
	// end allocator required interface

private:
	const char* name;
};