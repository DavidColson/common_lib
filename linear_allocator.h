#pragma once

#include <stdint.h>

#define DEFAULT_RESERVE 268435456 // 256 megabytes

struct LinearAllocator {
	// begin allocator required interface
	LinearAllocator();
	LinearAllocator(const char* _name);

	// Instance must not be copied, use LinearAllocatorRef instead
	LinearAllocator(const LinearAllocator& copy) = delete;

	void* 	Allocate(size_t size);
	void* 	Reallocate(void* ptr, size_t size, size_t oldSize);
	void 	Free(void* ptr);

	const char* GetName() { return name; }
	void 		SetName(const char* _name) { name = _name; }
	// end allocator required interface
	
	void Init(size_t defaultReserve = DEFAULT_RESERVE);
	void Reset(bool stampMemory = false);
	void Finished();
	void ExpandCommitted(uint8_t* pDesiredEnd);

	const char* name;
	size_t alignment{ 8 };
	size_t pageSize { 0 };
	size_t reserveSize { 0 };

	uint8_t* pMemoryBase { nullptr };
	uint8_t* pCurrentHead { nullptr };
	uint8_t* pFirstUncommittedPage { nullptr };
	uint8_t* pAddressLimit { nullptr };
};

struct LinearAllocatorRef {
	// begin allocator required interface
	LinearAllocatorRef() {}
	LinearAllocatorRef(LinearAllocator& pAlloc) { pAllocator = &pAlloc; }

	void* 	Allocate(size_t size) { return pAllocator->Allocate(size); }
	void* 	Reallocate(void* ptr, size_t size, size_t oldSize) { return pAllocator->Reallocate(ptr, size, oldSize); }
	void 	Free(void* ptr) { pAllocator->Free(ptr); }

	const char* GetName() { pAllocator->GetName(); }
	void 		SetName(const char* _name) {}
	// end allocator required interface
	LinearAllocator* pAllocator{ nullptr };
};