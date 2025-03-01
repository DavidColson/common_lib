// Copyright 2020-2022 David Colson. All rights reserved.

struct Allocation {
    void* pointer { nullptr };
    void* pAllocator { nullptr };
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

namespace {
MemoryTrackerState* g_pCtx { nullptr };
};

// ***********************************************************************

void InitContext() {
	Arena* pArena = ArenaCreate();
    g_pCtx = New(pArena, MemoryTrackerState);
	g_pCtx->pArena = pArena;
    g_pCtx->allocationTable.pArena = pArena;
}

// ***********************************************************************

void CheckMalloc(void* pAllocatorPtr, void* pAllocated, u64 size) {
    if (g_pCtx == nullptr)
        InitContext();

    Allocation allocation;
    allocation.pointer = pAllocated;
    allocation.pAllocator = pAllocatorPtr;
    allocation.size = size;
    allocation.isLive = true;
    allocation.allocStackTraceFrames = PlatformDebug::CollectStackTrace(allocation.allocStackTrace, 100, 2);
    g_pCtx->allocationTable[pAllocated] = allocation;
}

// ***********************************************************************

void CheckRealloc(void* pAllocatorPtr, void* pAllocated, void* ptr, u64 size, u64 oldSize) {
    if (g_pCtx == nullptr)
        InitContext();

    if (Allocation* alloc = g_pCtx->allocationTable.Get(ptr)) {  // pre-existing allocation
        if (alloc->pAllocator != pAllocatorPtr)
            __debugbreak();

        if (alloc->pointer != pAllocated) {  // Memory has changed location, so we must change the key
            // old alloc is effectively freed
            alloc->isLive = false;
            alloc->freeStackTraceFrames = PlatformDebug::CollectStackTrace(alloc->freeStackTrace, 100, 2);

            Allocation newAlloc;
            newAlloc.pointer = pAllocated;
            newAlloc.pAllocator = pAllocatorPtr;
            newAlloc.size = size;
            newAlloc.isLive = true;
            newAlloc.allocStackTraceFrames = PlatformDebug::CollectStackTrace(newAlloc.allocStackTrace, 100, 2);
            g_pCtx->allocationTable[pAllocated] = newAlloc;
        } else {
            alloc->size = size;
        }
    } else {  // new allocation
        Allocation allocation;
        allocation.pointer = pAllocated;
        allocation.pAllocator = pAllocatorPtr;
        allocation.size = size;
        allocation.isLive = true;
        allocation.allocStackTraceFrames = PlatformDebug::CollectStackTrace(allocation.allocStackTrace, 100);
        g_pCtx->allocationTable[pAllocated] = allocation;
    }
}

// ***********************************************************************

void ReportDoubleFree(Allocation& alloc, void** newFreeTrace, u64 newFreeTraceFrames) {
    String allocTrace = PlatformDebug::PrintStackTraceToString(alloc.allocStackTrace, alloc.allocStackTraceFrames, g_pCtx->pArena);
    String trace = PlatformDebug::PrintStackTraceToString(alloc.freeStackTrace, alloc.freeStackTraceFrames, g_pCtx->pArena);
    String trace2 = PlatformDebug::PrintStackTraceToString(newFreeTrace, newFreeTraceFrames, g_pCtx->pArena);

    Log::Warn("------ Hey idiot, detected double free at %p. Fix your shit! ------\nAllocated At:\n%s\nPreviously Freed At: \n%s\nFreed Again At:\n%s", alloc.pointer, allocTrace.pData, trace.pData, trace2.pData);
    __debugbreak();
}

// ***********************************************************************

void ReportUnknownFree(void* ptr) {
    Log::Warn("\n------ Hey idiot, detected free of untracked memory %p. Fix your shit! ------\n", ptr);
    __debugbreak();
}

// ***********************************************************************

void CheckFree(void* pAllocatorPtr, void* ptr) {
    if (g_pCtx == nullptr)
        InitContext();

    if (Allocation* alloc = g_pCtx->allocationTable.Get(ptr)) {
        if (alloc->pAllocator != pAllocatorPtr)
            __debugbreak();  // Alloc/Free allocator mismatch TODO: Report an error

        if (!alloc->isLive) {
            void* stackTrace[100];
            u64 stackFrames = PlatformDebug::CollectStackTrace(stackTrace, 100);
            ReportDoubleFree(*alloc, stackTrace, stackFrames);
        }

        alloc->isLive = false;
        alloc->freeStackTraceFrames = PlatformDebug::CollectStackTrace(alloc->freeStackTrace, 100);
    } else {
        ReportUnknownFree(ptr);
    }
}

// ***********************************************************************

void MarkNotALeak(void* ptr) {
	if (g_pCtx == nullptr)
		InitContext();

	if (Allocation* alloc = g_pCtx->allocationTable.Get(ptr)) {
		alloc->notALeak = true;
	}
}

// ***********************************************************************

int ReportMemoryLeaks() {
#ifdef MEMORY_TRACKING
    if (g_pCtx == nullptr)
        return 0;

    int leakCounter = 0;
    for (size i = 0; i < g_pCtx->allocationTable.tableSize; i++) {
        if (g_pCtx->allocationTable.pTable[i].hash != UNUSED_HASH) {
            Allocation& alloc = g_pCtx->allocationTable.pTable[i].value;
            if (alloc.isLive && !alloc.notALeak) {
                leakCounter++;
                String trace = PlatformDebug::PrintStackTraceToString(alloc.allocStackTrace, alloc.allocStackTraceFrames, g_pCtx->pArena);
                Log::Warn(" ------ Oi dimwit, detected memory leak at address %p of size %zi. Fix your shit! ------\n%s", alloc.pointer, alloc.size, trace.pData);
            }
        }
    }
    return leakCounter;
#else
    return 0;
#endif
}
