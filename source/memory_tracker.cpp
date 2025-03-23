// Copyright 2020-2022 David Colson. All rights reserved.

struct Allocation {
    void* pointer { nullptr };
    u64 size { 0 };
    bool isLive { false };
	bool notALeak { false };
    void* allocStackTrace[100];
    u64 allocStackTraceFrames { 0 };
    void* freeStackTrace[100];
    u64 freeStackTraceFrames { 0 };
};

struct MemoryTrackerState {
	Arena* pArena;
    HashMap<void*, Allocation> allocationTable;
};

static MemoryTrackerState* pMemTrack { nullptr };

// ***********************************************************************

void InitContext() {
	Arena* pArena = ArenaCreate(DEFAULT_RESERVE, true);
    pMemTrack = New(pArena, MemoryTrackerState);
	pMemTrack->pArena = pArena;
    pMemTrack->allocationTable.pArena = pArena;
}

// ***********************************************************************

void CheckMalloc(void* pAllocated, u64 size) {
    if (pMemTrack == nullptr)
        InitContext();

    Allocation allocation;
    allocation.pointer = pAllocated;
    allocation.size = size;
    allocation.isLive = true;
    allocation.allocStackTraceFrames = Debug::CollectStackTrace(allocation.allocStackTrace, 100, 2);
    pMemTrack->allocationTable[pAllocated] = allocation;
}

// ***********************************************************************

void CheckRealloc(void* pAllocated, void* ptr, u64 size, u64 oldSize) {
    if (pMemTrack == nullptr)
        InitContext();

    if (Allocation* alloc = pMemTrack->allocationTable.Get(ptr)) {  // pre-existing allocation
        if (alloc->pointer != pAllocated) {  // Memory has changed location, so we must change the key
            // old alloc is effectively freed
            alloc->isLive = false;
            alloc->freeStackTraceFrames = Debug::CollectStackTrace(alloc->freeStackTrace, 100, 2);

            Allocation newAlloc;
            newAlloc.pointer = pAllocated;
            newAlloc.size = size;
            newAlloc.isLive = true;
            newAlloc.allocStackTraceFrames = Debug::CollectStackTrace(newAlloc.allocStackTrace, 100, 2);
            pMemTrack->allocationTable[pAllocated] = newAlloc;
        } else {
            alloc->size = size;
        }
    } else {  // new allocation
        Allocation allocation;
        allocation.pointer = pAllocated;
        allocation.size = size;
        allocation.isLive = true;
        allocation.allocStackTraceFrames = Debug::CollectStackTrace(allocation.allocStackTrace, 100);
        pMemTrack->allocationTable[pAllocated] = allocation;
    }
}

// ***********************************************************************

void ReportDoubleFree(Allocation& alloc, void** newFreeTrace, u64 newFreeTraceFrames) {
    String allocTrace = Debug::PrintStackTraceToString(alloc.allocStackTrace, alloc.allocStackTraceFrames, pMemTrack->pArena);
    String trace = Debug::PrintStackTraceToString(alloc.freeStackTrace, alloc.freeStackTraceFrames, pMemTrack->pArena);
    String trace2 = Debug::PrintStackTraceToString(newFreeTrace, newFreeTraceFrames, pMemTrack->pArena);

    Log::Warn("------ Hey idiot, detected double free at %p. Fix your shit! ------\nAllocated At:\n%s\nPreviously Freed At: \n%s\nFreed Again At:\n%s", alloc.pointer, allocTrace.pData, trace.pData, trace2.pData);
    __debugbreak();
}

// ***********************************************************************

void ReportUnknownFree(void* ptr) {
    Log::Warn("\n------ Hey idiot, detected free of untracked memory %p. Fix your shit! ------\n", ptr);
    __debugbreak();
}

// ***********************************************************************

void CheckFree(void* ptr) {
    if (pMemTrack == nullptr)
        InitContext();

    if (Allocation* alloc = pMemTrack->allocationTable.Get(ptr)) {
        if (!alloc->isLive) {
            void* stackTrace[100];
            u64 stackFrames = Debug::CollectStackTrace(stackTrace, 100);
            ReportDoubleFree(*alloc, stackTrace, stackFrames);
        }

        alloc->isLive = false;
        alloc->freeStackTraceFrames = Debug::CollectStackTrace(alloc->freeStackTrace, 100);
    } else {
        ReportUnknownFree(ptr);
    }
}

// ***********************************************************************

void MarkNotALeak(void* ptr) {
	if (pMemTrack == nullptr)
		InitContext();

	if (Allocation* alloc = pMemTrack->allocationTable.Get(ptr)) {
		alloc->notALeak = true;
	}
}

// ***********************************************************************

int ReportMemoryLeaks() {
#ifdef MEMORY_TRACKING
    if (pMemTrack == nullptr)
        return 0;

    int leakCounter = 0;
    for (i64 i = 0; i < pMemTrack->allocationTable.tableSize; i++) {
        if (pMemTrack->allocationTable.pTable[i].hash != UNUSED_HASH) {
            Allocation& alloc = pMemTrack->allocationTable.pTable[i].value;
            if (alloc.isLive && !alloc.notALeak) {
                leakCounter++;
                String trace = Debug::PrintStackTraceToString(alloc.allocStackTrace, alloc.allocStackTraceFrames, pMemTrack->pArena);
                Log::Warn(" ------ Oi dimwit, detected memory leak at address %p of size %zi. Fix your shit! ------\n%s", alloc.pointer, alloc.size, trace.pData);
            }
        }
    }
    return leakCounter;
#else
    return 0;
#endif
}

// ***********************************************************************

void ReportMemoryUsage() {
#ifdef MEMORY_TRACKING
    if (pMemTrack == nullptr)
        return;

	i64 memoryAllocated = 0;
    for (i64 i = 0; i < pMemTrack->allocationTable.tableSize; i++) {
        if (pMemTrack->allocationTable.pTable[i].hash != UNUSED_HASH) {
            Allocation& alloc = pMemTrack->allocationTable.pTable[i].value;
			if (alloc.isLive) {
				memoryAllocated += alloc.size;
			}
		}
	}
	Log::Info("Tracked memory usage: %d (bytes) %f (kbytes) %f (mbytes)", memoryAllocated, (f32)memoryAllocated/1024, (f32)memoryAllocated/1024/1024);
#endif
}
