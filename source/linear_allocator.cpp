// Copyright 2020-2022 David Colson. All rights reserved.

#include "linear_allocator.h"

#include "log.h"
#include "memory.h"

#include <stdio.h>
#include <string.h>

// Windows defines
// ***********************************************************************

extern "C" {
#define WIN(r) __declspec(dllimport) r __stdcall

typedef struct _SYSTEM_INFO {
    union {
        u32 dwOemId;          // Obsolete field...do not use
        struct {
            u16 wProcessorArchitecture;
            u16 wReserved;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
    u32 dwPageSize;
    void* lpMinimumApplicationAddress;
    void* lpMaximumApplicationAddress;
    u32* dwActiveProcessorMask;
    u32 dwNumberOfProcessors;
    u32 dwProcessorType;
    u32 dwAllocationGranularity;
    u16 wProcessorLevel;
    u16 wProcessorRevision;
} SYSTEM_INFO, *LPSYSTEM_INFO;

WIN(void) GetSystemInfo(LPSYSTEM_INFO lpSystemInfo);

#define MEM_COMMIT                      0x00001000  
#define MEM_RESERVE                     0x00002000  
#define MEM_RELEASE                     0x00008000  
#define PAGE_READWRITE          0x04 

WIN(void*) VirtualAlloc(void* lpAddress, usize dwSize, u32 flAllocationType, u32 flProtect);
WIN(u32) VirtualFree(void* lpAddress,usize dwSize, u32 dwFreeType);
}

// ***********************************************************************

usize Align(usize toAlign, usize alignment) {
    // Rounds up to nearest multiple of alignment (works if alignment is power of 2)
    return (toAlign + alignment - 1) & ~(alignment - 1);
}

// ***********************************************************************

u8* AlignPtr(u8* toAlign, usize alignment) {
    return (u8*)Align(usize(toAlign), alignment);
}

// ***********************************************************************

void* LinearAllocator::Allocate(usize size) {
    if (pMemoryBase == nullptr)
        Init();

    u8* pOutput = AlignPtr(pCurrentHead, alignment);
    u8* pEnd = pOutput + size;

    if (pEnd > pFirstUncommittedPage) {
        Assert(pEnd < pAddressLimit);
        ExpandCommitted(pEnd);
    }

    pCurrentHead = pEnd;
    return (void*)pOutput;
}

// ***********************************************************************

void* LinearAllocator::Reallocate(void* ptr, usize size, usize oldSize) {
    u8* pOutput = (u8*)Allocate(size);
    usize sizeToCopy = size < oldSize ? size : oldSize;
    memcpy(pOutput, ptr, sizeToCopy);
    return (void*)pOutput;
}

// ***********************************************************************

void LinearAllocator::Free(void* ptr) {
    // Do nothing
}

// ***********************************************************************

void LinearAllocator::Init(usize defaultReserve) {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    pageSize = sysInfo.dwPageSize;

    reserveSize = Align(defaultReserve, pageSize);
    pMemoryBase = (u8*)VirtualAlloc(nullptr, reserveSize, MEM_RESERVE, PAGE_READWRITE);
    Assert(pMemoryBase != nullptr);

#ifdef MEMORY_TRACKING
	CheckMalloc(this, pMemoryBase, reserveSize);
#endif

    pFirstUncommittedPage = pMemoryBase;
    pAddressLimit = pMemoryBase + reserveSize;
    pCurrentHead = pMemoryBase;
}

// ***********************************************************************

void LinearAllocator::Reset(bool stampMemory) {
    if (pMemoryBase == nullptr)
        return;

    // To debug memory corruption, probably have on in debug builds
    if (stampMemory)
        memset(pMemoryBase, 0xcc, pCurrentHead - pMemoryBase);

    pCurrentHead = pMemoryBase;
}

// ***********************************************************************

void LinearAllocator::Finished() {
    if (pMemoryBase != nullptr) {
#ifdef MEMORY_TRACKING
        CheckFree(this, pMemoryBase);
#endif
        VirtualFree(pMemoryBase, 0, MEM_RELEASE);
    }
}

// ***********************************************************************

void LinearAllocator::ExpandCommitted(u8* pDesiredEnd) {
    usize currentSpace = pFirstUncommittedPage - pMemoryBase;
    usize requiredSpace = pDesiredEnd - pFirstUncommittedPage;
    Assert(requiredSpace > 0);

    usize size = Align(requiredSpace, pageSize);
    VirtualAlloc(pFirstUncommittedPage, size, MEM_COMMIT, PAGE_READWRITE);
#ifdef MEMORY_TRACKING
    CheckRealloc(this, pMemoryBase, pMemoryBase, currentSpace + size, currentSpace);
#endif
    pFirstUncommittedPage += size;
}
