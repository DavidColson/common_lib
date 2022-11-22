#include "memory_tracker.h"

#include "hashmap.h"

struct ForceNoTrackAllocator {
	ForceNoTrackAllocator() {}
	ForceNoTrackAllocator(const char* _name) {}

	void* 	Allocate(size_t size) { return malloc(size); }
	void* 	Reallocate(void* ptr, size_t size) { return realloc(ptr, size); }
	void 	Free(void* ptr) { free(ptr); }

	const char* GetName() { return ""; }
	void  		SetName(const char* _name) {}
};

// store global mem tracking state, initialize on first call

struct Allocation {
	void* pointer { nullptr };
	size_t size { 0 };
	bool isLive { false };
};

struct MemoryTrackerState {
	HashMap<void*, Allocation, ForceNoTrackAllocator> allocationMap;
};

namespace {
	MemoryTrackerState* pCtx { nullptr };
};

void* mallocTrack(size_t size) {
	if (pCtx == nullptr)
	{
		pCtx = (MemoryTrackerState*)malloc(sizeof(MemoryTrackerState));
		SYS_P_NEW(pCtx) MemoryTrackerState();
	}

	// do the alloc, and then store it in the hashmap
	void* pMemory = malloc(size);

	Allocation allocation;
	allocation.pointer = pMemory;
	allocation.size = size;
	allocation.isLive = true;

	pCtx->allocationMap[pMemory] = allocation;

	return pMemory;
}

void* reallocTrack(void* ptr, size_t size) {
	return realloc(ptr, size);
}

void freeTrack(void* ptr) {
	return free(ptr);
}