#include "linear_allocator.h"

#include "memory.h"

#include <stdio.h>
#include <Windows.h>
#include <Memoryapi.h>

#ifdef _DEBUG
#define DEBUG_CHECK(expression) if (!(expression)) __debugbreak();
#else
#define DEBUG_CHECK(expression)
#endif

size_t Align(size_t toAlign, size_t alignment) {
	// Rounds up to nearest multiple of alignment (works if alignment is power of 2)
	return (toAlign + alignment-1) & ~(alignment-1);
}

uint8_t* AlignPtr(uint8_t* toAlign, size_t alignment) {
	return (uint8_t*)Align(size_t(toAlign), alignment);
}

LinearAllocator::LinearAllocator() {

}

LinearAllocator::LinearAllocator(const char* _name) {
	name = _name;
}

void* LinearAllocator::Allocate(size_t size) {
	if (pMemoryBase == nullptr) Init();

	uint8_t* pOutput = AlignPtr(pCurrentHead, alignment);
	uint8_t* pEnd = pOutput + size;

	if (pEnd > pFirstUncommittedPage) {
		DEBUG_CHECK(pEnd < pAddressLimit);
		ExpandCommitted(pEnd);
	}

	pCurrentHead = pEnd;
	return (void*)pOutput;
}

void* LinearAllocator::Reallocate(void* ptr, size_t size, size_t oldSize) {
	uint8_t* pOutput = (uint8_t*)Allocate(size);
	size_t sizeToCopy = size < oldSize ? size : oldSize;
	memcpy(pOutput, ptr, sizeToCopy);
	return (void*)pOutput;
}

void LinearAllocator::Free(void* ptr) {
	// Do nothing
}

void LinearAllocator::Init(size_t defaultReserve) {
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	pageSize = sysInfo.dwPageSize;

	reserveSize = Align(defaultReserve, pageSize);
	pMemoryBase = (uint8_t*)VirtualAlloc(nullptr, reserveSize, MEM_RESERVE, PAGE_READWRITE);
	DEBUG_CHECK(pMemoryBase != nullptr);

	pFirstUncommittedPage = pMemoryBase;
	pAddressLimit = pMemoryBase + reserveSize;
	pCurrentHead = pMemoryBase;
}

void LinearAllocator::Reset(bool stampMemory) {
	if (pMemoryBase == nullptr) return;

	// To debug memory corruption, probably have on in debug builds
	if (stampMemory)
		memset(pMemoryBase, 0xcc, pCurrentHead - pMemoryBase);

	pCurrentHead = pMemoryBase;
}

void LinearAllocator::Finished() {
	CheckFree(this, pMemoryBase);
	VirtualFree(pMemoryBase, 0, MEM_RELEASE);
}

void LinearAllocator::ExpandCommitted(uint8_t* pDesiredEnd) {
	size_t currentSpace = pFirstUncommittedPage - pMemoryBase;
	size_t requiredSpace = pDesiredEnd - pFirstUncommittedPage;
	DEBUG_CHECK(requiredSpace > 0);

	size_t size = Align(requiredSpace, pageSize);
	VirtualAlloc(pFirstUncommittedPage, size, MEM_COMMIT, PAGE_READWRITE);
	CheckRealloc(this, pMemoryBase, pMemoryBase, currentSpace + size, currentSpace);
	pFirstUncommittedPage += size;
}