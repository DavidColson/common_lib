// Copyright 2020-2022 David Colson. All rights reserved.

#include "linear_allocator.h"

#include "log.h"
#include "memory.h"

#include <windows.h>
#include <Memoryapi.h>
#include <stdio.h>

// ***********************************************************************

size_t Align(size_t toAlign, size_t alignment) {
    // Rounds up to nearest multiple of alignment (works if alignment is power of 2)
    return (toAlign + alignment - 1) & ~(alignment - 1);
}

// ***********************************************************************

uint8_t* AlignPtr(uint8_t* toAlign, size_t alignment) {
    return (uint8_t*)Align(size_t(toAlign), alignment);
}

// ***********************************************************************

void* LinearAllocator::Allocate(size_t size) {
    if (m_pMemoryBase == nullptr)
        Init();

    uint8_t* pOutput = AlignPtr(m_pCurrentHead, m_alignment);
    uint8_t* pEnd = pOutput + size;

    if (pEnd > m_pFirstUncommittedPage) {
        Assert(pEnd < m_pAddressLimit);
        ExpandCommitted(pEnd);
    }

    m_pCurrentHead = pEnd;
    return (void*)pOutput;
}

// ***********************************************************************

void* LinearAllocator::Reallocate(void* ptr, size_t size, size_t oldSize) {
    uint8_t* pOutput = (uint8_t*)Allocate(size);
    size_t sizeToCopy = size < oldSize ? size : oldSize;
    memcpy(pOutput, ptr, sizeToCopy);
    return (void*)pOutput;
}

// ***********************************************************************

void LinearAllocator::Free(void* ptr) {
    // Do nothing
}

// ***********************************************************************

void LinearAllocator::Init(size_t defaultReserve) {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    m_pageSize = sysInfo.dwPageSize;

    m_reserveSize = Align(defaultReserve, m_pageSize);
    m_pMemoryBase = (uint8_t*)VirtualAlloc(nullptr, m_reserveSize, MEM_RESERVE, PAGE_READWRITE);
    Assert(m_pMemoryBase != nullptr);

    m_pFirstUncommittedPage = m_pMemoryBase;
    m_pAddressLimit = m_pMemoryBase + m_reserveSize;
    m_pCurrentHead = m_pMemoryBase;
}

// ***********************************************************************

void LinearAllocator::Reset(bool stampMemory) {
    if (m_pMemoryBase == nullptr)
        return;

    // To debug memory corruption, probably have on in debug builds
    if (stampMemory)
        memset(m_pMemoryBase, 0xcc, m_pCurrentHead - m_pMemoryBase);

    m_pCurrentHead = m_pMemoryBase;
}

// ***********************************************************************

void LinearAllocator::Finished() {
#ifdef MEMORY_TRACKING
    CheckFree(this, m_pMemoryBase);
#endif
    VirtualFree(m_pMemoryBase, 0, MEM_RELEASE);
}

// ***********************************************************************

void LinearAllocator::ExpandCommitted(uint8_t* pDesiredEnd) {
    size_t currentSpace = m_pFirstUncommittedPage - m_pMemoryBase;
    size_t requiredSpace = pDesiredEnd - m_pFirstUncommittedPage;
    Assert(requiredSpace > 0);

    size_t size = Align(requiredSpace, m_pageSize);
    VirtualAlloc(m_pFirstUncommittedPage, size, MEM_COMMIT, PAGE_READWRITE);
#ifdef MEMORY_TRACKING
    CheckRealloc(this, m_pMemoryBase, m_pMemoryBase, currentSpace + size, currentSpace);
#endif
    m_pFirstUncommittedPage += size;
}
