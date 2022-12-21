// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

#include "memory.h"

#include <stdint.h>

#define DEFAULT_RESERVE 268435456  // 256 megabytes

struct LinearAllocator : public IAllocator {
    virtual void* Allocate(size_t size) override;
    virtual void* Reallocate(void* ptr, size_t size, size_t oldSize) override;
    virtual void Free(void* ptr) override;

    void Init(size_t defaultReserve = DEFAULT_RESERVE);
    void Reset(bool stampMemory = false);
    void Finished();
    void ExpandCommitted(uint8_t* pDesiredEnd);

    const char* m_name;
    size_t m_alignment { 8 };
    size_t m_pageSize { 0 };
    size_t m_reserveSize { 0 };

    uint8_t* m_pMemoryBase { nullptr };
    uint8_t* m_pCurrentHead { nullptr };
    uint8_t* m_pFirstUncommittedPage { nullptr };
    uint8_t* m_pAddressLimit { nullptr };
};
