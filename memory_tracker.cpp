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

struct Allocation {
	void* pointer { nullptr };
	size_t size { 0 };
	bool isLive { false };
};

struct MemoryTrackerState {
	HashMap<void*, Allocation, ForceNoTrackAllocator> allocationTable;
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

	void* pMemory = malloc(size);

	Allocation allocation;
	allocation.pointer = pMemory;
	allocation.size = size;
	allocation.isLive = true;
	pCtx->allocationTable[pMemory] = allocation;

	return pMemory;
}

void* reallocTrack(void* ptr, size_t size) {
	if (pCtx == nullptr)
	{
		pCtx = (MemoryTrackerState*)malloc(sizeof(MemoryTrackerState));
		SYS_P_NEW(pCtx) MemoryTrackerState();
	}

	void* pMemory = realloc(ptr, size);

	if (pCtx->allocationTable.Exists(ptr)) { // pre-existing allocation
		Allocation& alloc = pCtx->allocationTable[ptr];

		if (alloc.pointer != pMemory) { // Memory has changed location, so we must change the key
			Allocation newAlloc = alloc;
			newAlloc.pointer = pMemory;
			newAlloc.size = size;
			pCtx->allocationTable.Erase(alloc.pointer);
			pCtx->allocationTable[pMemory] = newAlloc;
		}
		else
		{
			alloc.pointer = pMemory;
			alloc.size = size;
		}
	}
	else { // new allocation
		Allocation allocation;
		allocation.pointer = pMemory;
		allocation.size = size;
		allocation.isLive = true;
		pCtx->allocationTable[pMemory] = allocation;
		return pMemory;
	}

	return pMemory;
}

void freeTrack(void* ptr) {
	if (pCtx == nullptr)
	{
		pCtx = (MemoryTrackerState*)malloc(sizeof(MemoryTrackerState));
		SYS_P_NEW(pCtx) MemoryTrackerState();
	}

	if (pCtx->allocationTable.Exists(ptr)) {
		Allocation& alloc = pCtx->allocationTable[ptr];

		if (!alloc.isLive)
			__debugbreak(); // Double free

		alloc.isLive = false;
	}
	else {
		__debugbreak(); // Attempting to free some memory that was never allocated
	}
	free(ptr);
}