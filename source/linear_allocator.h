// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

#include "types.h"
#include "memory.h"

#define DEFAULT_RESERVE 268435456  // 256 megabytes

struct LinearAllocator : public IAllocator {
    virtual void* Allocate(usize size) override;
    virtual void* Reallocate(void* ptr, usize size, usize oldSize) override;
    virtual void Free(void* ptr) override;

    void Init(usize defaultReserve = DEFAULT_RESERVE);
    void Reset(bool stampMemory = false);
    void Finished();
    void ExpandCommitted(u8* pDesiredEnd);

    const byte* name;
    usize alignment { 16 };
    usize pageSize { 0 };
    usize reserveSize { 0 };

    u8* pMemoryBase { nullptr };
    u8* pCurrentHead { nullptr };
    u8* pFirstUncommittedPage { nullptr };
    u8* pAddressLimit { nullptr };
};
