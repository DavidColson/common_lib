#include "memory.h"

Allocator::Allocator() {
	name = "unknown allocator";
}

Allocator::Allocator(const char* _name) {
	name = _name;
}

void* Allocator::Allocate(size_t size) {
	return SYS_ALLOC(size);
}

void* Allocator::Reallocate(void* ptr, size_t size) {
	return SYS_REALLOC(ptr, size);
}

void Allocator::Free(void* ptr) {
	SYS_FREE(ptr);
}

const char* Allocator::GetName() {
	return name;
}

void Allocator::SetName(const char* _name) {
	name = _name;
}