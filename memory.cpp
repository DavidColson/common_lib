#include "memory.h"


void* DefaultAllocator::Allocate(size_t size) {
	return SYS_ALLOC(size);
}

void* DefaultAllocator::Reallocate(void* ptr, size_t size, size_t oldSize) {
	return SYS_REALLOC(ptr, size, oldSize);
}

void DefaultAllocator::Free(void* ptr) {
	SYS_FREE(ptr);
}