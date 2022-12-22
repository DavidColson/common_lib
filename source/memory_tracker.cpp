// Copyright 2020-2022 David Colson. All rights reserved.

#include "memory_tracker.h"

#include "defer.h"
#include "light_string.h"
#include "log.h"
#include "platform_debug.h"
#include "resizable_array.h"
#include "hashmap.inl"

struct ForceNoTrackAllocator : public IAllocator {
    void* Allocate(size_t size) {
        return malloc(size);
    }
    void* Reallocate(void* ptr, size_t size, size_t oldSize) {
        return realloc(ptr, size);
    }
    void Free(void* ptr) {
        free(ptr);
    }
};

struct Allocation {
    void* m_pointer { nullptr };
    void* m_pAllocator { nullptr };
    size_t m_size { 0 };
    bool m_isLive { false };
    void* m_allocStackTrace[100];
    size_t m_allocStackTraceFrames { 0 };
    void* m_freeStackTrace[100];
    size_t m_freeStackTraceFrames { 0 };
};

struct MemoryTrackerState {
    HashMap<void*, Allocation> m_allocationTable;
};

namespace {
ForceNoTrackAllocator g_noTrackAllocator;
MemoryTrackerState* g_pCtx { nullptr };
};

// ***********************************************************************

void InitContext() {
    g_pCtx = (MemoryTrackerState*)malloc(sizeof(MemoryTrackerState));
    SYS_P_NEW(g_pCtx)
    MemoryTrackerState();
    g_pCtx->m_allocationTable.m_pAlloc = &g_noTrackAllocator;
}

// ***********************************************************************

void* MallocWrap(size_t size) {
    void* pMemory = malloc(size);
    CheckMalloc(nullptr, pMemory, size);
    return pMemory;
}

// ***********************************************************************

void* ReallocWrap(void* ptr, size_t size, size_t oldSize) {
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

void CheckMalloc(void* pAllocatorPtr, void* pAllocated, size_t size) {
    if (g_pCtx == nullptr)
        InitContext();

    Allocation allocation;
    allocation.m_pointer = pAllocated;
    allocation.m_pAllocator = pAllocatorPtr;
    allocation.m_size = size;
    allocation.m_isLive = true;
    allocation.m_allocStackTraceFrames = PlatformDebug::CollectStackTrace(allocation.m_allocStackTrace, 100, 2);
    g_pCtx->m_allocationTable[pAllocated] = allocation;
}

// ***********************************************************************

void CheckRealloc(void* pAllocatorPtr, void* pAllocated, void* ptr, size_t size, size_t oldSize) {
    if (g_pCtx == nullptr)
        InitContext();

    if (Allocation* alloc = g_pCtx->m_allocationTable.Get(ptr)) {  // pre-existing allocation
        if (alloc->m_pAllocator != pAllocatorPtr)
            __debugbreak();

        if (alloc->m_pointer != pAllocated) {  // Memory has changed location, so we must change the key
            // old alloc is effectively freed
            alloc->m_isLive = false;
            alloc->m_freeStackTraceFrames = PlatformDebug::CollectStackTrace(alloc->m_freeStackTrace, 100, 2);

            Allocation newAlloc;
            newAlloc.m_pointer = pAllocated;
            newAlloc.m_pAllocator = pAllocatorPtr;
            newAlloc.m_size = size;
            newAlloc.m_isLive = true;
            newAlloc.m_allocStackTraceFrames = PlatformDebug::CollectStackTrace(newAlloc.m_allocStackTrace, 100, 2);
            g_pCtx->m_allocationTable[pAllocated] = newAlloc;
        } else {
            alloc->m_size = size;
        }
    } else {  // new allocation
        Allocation allocation;
        allocation.m_pointer = pAllocated;
        allocation.m_pAllocator = pAllocatorPtr;
        allocation.m_size = size;
        allocation.m_isLive = true;
        allocation.m_allocStackTraceFrames = PlatformDebug::CollectStackTrace(allocation.m_allocStackTrace, 100);
        g_pCtx->m_allocationTable[pAllocated] = allocation;
    }
}

// ***********************************************************************

void ReportDoubleFree(Allocation& alloc, void** newFreeTrace, size_t newFreeTraceFrames) {
    String allocTrace = PlatformDebug::PrintStackTraceToString(alloc.m_allocStackTrace, alloc.m_allocStackTraceFrames, &g_noTrackAllocator);
    defer(FreeString(allocTrace, &g_noTrackAllocator));

    String trace = PlatformDebug::PrintStackTraceToString(alloc.m_freeStackTrace, alloc.m_freeStackTraceFrames, &g_noTrackAllocator);
    defer(FreeString(trace, &g_noTrackAllocator));

    String trace2 = PlatformDebug::PrintStackTraceToString(newFreeTrace, newFreeTraceFrames, &g_noTrackAllocator);
    defer(FreeString(trace2, &g_noTrackAllocator));

    Log::Warn("------ Hey idiot, detected double free at %p. Fix your shit! ------\nAllocated At:\n%s\nPreviously Freed At: \n%s\nFreed Again At:\n%s", alloc.m_pointer, allocTrace.m_pData, trace.m_pData, trace2.m_pData);
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

    if (Allocation* alloc = g_pCtx->m_allocationTable.Get(ptr)) {
        if (alloc->m_pAllocator != pAllocatorPtr)
            __debugbreak();  // Alloc/Free allocator mismatch TODO: Report an error

        if (!alloc->m_isLive) {
            void* stackTrace[100];
            size_t stackFrames = PlatformDebug::CollectStackTrace(stackTrace, 100);
            ReportDoubleFree(*alloc, stackTrace, stackFrames);
        }

        alloc->m_isLive = false;
        alloc->m_freeStackTraceFrames = PlatformDebug::CollectStackTrace(alloc->m_freeStackTrace, 100);
    } else {
        ReportUnknownFree(ptr);
    }
}

// ***********************************************************************

int ReportMemoryLeaks() {
#ifdef MEMORY_TRACKING
    if (g_pCtx == nullptr)
        return 0;

    int leakCounter = 0;
    for (size_t i = 0; i < g_pCtx->m_allocationTable.m_tableSize; i++) {
        if (g_pCtx->m_allocationTable.m_pTable[i].hash != UNUSED_HASH) {
            Allocation& alloc = g_pCtx->m_allocationTable.m_pTable[i].value;
            if (alloc.m_isLive) {
                leakCounter++;
                String trace = PlatformDebug::PrintStackTraceToString(alloc.m_allocStackTrace, alloc.m_allocStackTraceFrames, &g_noTrackAllocator);
                defer(FreeString(trace, &g_noTrackAllocator));
                Log::Warn(" ------ Oi dimwit, detected memory leak at address %p of size %zi. Fix your shit! ------\n%s", alloc.m_pointer, alloc.m_size, trace.m_pData);
            }
        }
    }
    return leakCounter;
#else
    return 0;
#endif
}
