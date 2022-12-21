#include "memory_tracker.h"

#include "defer.h"
#include "hashmap.h"
#include "light_string.h"
#include "log.h"
#include "platform_debug.h"
#include "resizable_array.h"

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
    void* pointer { nullptr };
    void* pAllocator { nullptr };
    size_t size { 0 };
    bool isLive { false };
    void* allocStackTrace[100];
    size_t allocStackTraceFrames { 0 };
    void* freeStackTrace[100];
    size_t freeStackTraceFrames { 0 };
};

struct MemoryTrackerState {
    HashMap<void*, Allocation> allocationTable;
};

namespace {
ForceNoTrackAllocator noTrackAllocator;
MemoryTrackerState* pCtx { nullptr };
};

void InitContext() {
    pCtx = (MemoryTrackerState*)malloc(sizeof(MemoryTrackerState));
    SYS_P_NEW(pCtx)
    MemoryTrackerState();
    pCtx->allocationTable.pAlloc = &noTrackAllocator;
}

void* MallocWrap(size_t size) {
    void* pMemory = malloc(size);
    CheckMalloc(nullptr, pMemory, size);
    return pMemory;
}

void* ReallocWrap(void* ptr, size_t size, size_t oldSize) {
    void* pMemory = realloc(ptr, size);
    CheckRealloc(nullptr, pMemory, ptr, size, oldSize);
    return pMemory;
}

void FreeWrap(void* ptr) {
    CheckFree(nullptr, ptr);
    free(ptr);
}

void CheckMalloc(void* pAllocatorPtr, void* pAllocated, size_t size) {
    if (pCtx == nullptr)
        InitContext();

    Allocation allocation;
    allocation.pointer = pAllocated;
    allocation.pAllocator = pAllocatorPtr;
    allocation.size = size;
    allocation.isLive = true;
    allocation.allocStackTraceFrames = PlatformDebug::CollectStackTrace(allocation.allocStackTrace, 100, 2);
    pCtx->allocationTable[pAllocated] = allocation;
}

void CheckRealloc(void* pAllocatorPtr, void* pAllocated, void* ptr, size_t size, size_t oldSize) {
    if (pCtx == nullptr)
        InitContext();

    if (Allocation* alloc = pCtx->allocationTable.Get(ptr)) {  // pre-existing allocation
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
            pCtx->allocationTable[pAllocated] = newAlloc;
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
        pCtx->allocationTable[pAllocated] = allocation;
    }
}

void ReportDoubleFree(Allocation& alloc, void** newFreeTrace, size_t newFreeTraceFrames) {
    String allocTrace = PlatformDebug::PrintStackTraceToString(alloc.allocStackTrace, alloc.allocStackTraceFrames, &noTrackAllocator);
    defer(FreeString(allocTrace, &noTrackAllocator));

    String trace = PlatformDebug::PrintStackTraceToString(alloc.freeStackTrace, alloc.freeStackTraceFrames, &noTrackAllocator);
    defer(FreeString(trace, &noTrackAllocator));

    String trace2 = PlatformDebug::PrintStackTraceToString(newFreeTrace, newFreeTraceFrames, &noTrackAllocator);
    defer(FreeString(trace2, &noTrackAllocator));

    Log::Warn("------ Hey idiot, detected double free at %p. Fix your shit! ------\nAllocated At:\n%s\nPreviously Freed At: \n%s\nFreed Again At:\n%s", alloc.pointer, allocTrace.pData, trace.pData, trace2.pData);
    __debugbreak();
}

void ReportUnknownFree(void* ptr) {
    Log::Warn("\n------ Hey idiot, detected free of untracked memory %p. Fix your shit! ------\n", ptr);
    __debugbreak();
}

void CheckFree(void* pAllocatorPtr, void* ptr) {
    if (pCtx == nullptr)
        InitContext();

    if (Allocation* alloc = pCtx->allocationTable.Get(ptr)) {
        if (alloc->pAllocator != pAllocatorPtr)
            __debugbreak();  // Alloc/Free allocator mismatch TODO: Report an error

        if (!alloc->isLive) {
            void* stackTrace[100];
            size_t stackFrames = PlatformDebug::CollectStackTrace(stackTrace, 100);
            ReportDoubleFree(*alloc, stackTrace, stackFrames);
        }

        alloc->isLive = false;
        alloc->freeStackTraceFrames = PlatformDebug::CollectStackTrace(alloc->freeStackTrace, 100);
    } else {
        ReportUnknownFree(ptr);
    }
}

int ReportMemoryLeaks() {
#ifdef MEMORY_TRACKING
    if (pCtx == nullptr)
        return 0;

    int leakCounter = 0;
    for (size_t i = 0; i < pCtx->allocationTable.tableSize; i++) {
        if (pCtx->allocationTable.pTable[i].hash != UNUSED_HASH) {
            Allocation& alloc = pCtx->allocationTable.pTable[i].value;
            if (alloc.isLive) {
                leakCounter++;
                String trace = PlatformDebug::PrintStackTraceToString(alloc.allocStackTrace, alloc.allocStackTraceFrames, &noTrackAllocator);
                defer(FreeString(trace, &noTrackAllocator));
                Log::Warn(" ------ Oi dimwit, detected memory leak at address %p of size %zi. Fix your shit! ------\n%s", alloc.pointer, alloc.size, trace.pData);
            }
        }
    }
    return leakCounter;
#else
    return 0;
#endif
}
