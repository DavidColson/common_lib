// Copyright 2020-2022 David Colson. All rights reserved.

#include "memory_tracker.h"

#include "defer.h"
#include "light_string.h"
#include "log.h"
#include "platform_debug.h"
#include "resizable_array.h"
#include "hashmap.inl"

struct ForceNoTrackAllocator : public IAllocator {
    void* Allocate(usize size) {
        return malloc(size);
    }
    void* Reallocate(void* ptr, usize size, usize oldSize) {
        return realloc(ptr, size);
    }
    void Free(void* ptr) {
        free(ptr);
    }
};

struct Allocation {
    void* pointer { nullptr };
    void* pAllocator { nullptr };
    usize size { 0 };
    bool isLive { false };
	bool notALeak { false };
    void* allocStackTrace[100];
    usize allocStackTraceFrames { 0 };
    void* freeStackTrace[100];
    usize freeStackTraceFrames { 0 };
};

struct MemoryTrackerState {
    HashMap<void*, Allocation> allocationTable;
};

namespace {
ForceNoTrackAllocator g_noTrackAllocator;
MemoryTrackerState* g_pCtx { nullptr };
};

// ***********************************************************************

void InitContext() {
    g_pCtx = (MemoryTrackerState*)malloc(sizeof(MemoryTrackerState));
    SYS_P_NEW(g_pCtx) MemoryTrackerState();
    g_pCtx->allocationTable.pAlloc = &g_noTrackAllocator;
}

// ***********************************************************************

void* MallocWrap(usize size) {
    void* pMemory = malloc(size);
    CheckMalloc(nullptr, pMemory, size);
    return pMemory;
}

// ***********************************************************************

void* ReallocWrap(void* ptr, usize size, usize oldSize) {
    void* pMemory = realloc(ptr, size);
    CheckRealloc(nullptr, pMemory, ptr, size, oldSize);
    return pMemory;
}

// ***********************************************************************

void FreeWrap(void* ptr) {
    CheckFree(nullptr, ptr);
    free(ptr);
}

// ***********************************************************************

void CheckMalloc(void* pAllocatorPtr, void* pAllocated, usize size) {
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

void CheckRealloc(void* pAllocatorPtr, void* pAllocated, void* ptr, usize size, usize oldSize) {
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

void Reportf64Free(Allocation& alloc, void** newFreeTrace, usize newFreeTraceFrames) {
    String allocTrace = PlatformDebug::PrintStackTraceToString(alloc.allocStackTrace, alloc.allocStackTraceFrames, &g_noTrackAllocator);
    defer(FreeString(allocTrace, &g_noTrackAllocator));

    String trace = PlatformDebug::PrintStackTraceToString(alloc.freeStackTrace, alloc.freeStackTraceFrames, &g_noTrackAllocator);
    defer(FreeString(trace, &g_noTrackAllocator));

    String trace2 = PlatformDebug::PrintStackTraceToString(newFreeTrace, newFreeTraceFrames, &g_noTrackAllocator);
    defer(FreeString(trace2, &g_noTrackAllocator));

    Log::Warn("------ Hey idiot, detected f64 free at %p. Fix your shit! ------\nAllocated At:\n%s\nPreviously Freed At: \n%s\nFreed Again At:\n%s", alloc.pointer, allocTrace.pData, trace.pData, trace2.pData);
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
            usize stackFrames = PlatformDebug::CollectStackTrace(stackTrace, 100);
            Reportf64Free(*alloc, stackTrace, stackFrames);
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
    for (usize i = 0; i < g_pCtx->allocationTable.tableSize; i++) {
        if (g_pCtx->allocationTable.pTable[i].hash != UNUSED_HASH) {
            Allocation& alloc = g_pCtx->allocationTable.pTable[i].value;
            if (alloc.isLive && !alloc.notALeak) {
                leakCounter++;
                String trace = PlatformDebug::PrintStackTraceToString(alloc.allocStackTrace, alloc.allocStackTraceFrames, &g_noTrackAllocator);
                defer(FreeString(trace, &g_noTrackAllocator));
                Log::Warn(" ------ Oi dimwit, detected memory leak at address %p of size %zi. Fix your shit! ------\n%s", alloc.pointer, alloc.size, trace.pData);
            }
        }
    }
    return leakCounter;
#else
    return 0;
#endif
}
