// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

#include "types.h"
#include "memory.h"

// Resizable Array Structure
// -----------------------
// Must be freed manually
// ... todo documentation, examples, reasoning etc

template<typename Type>
struct ResizableArray {
    Type* pData { nullptr };
    u32 count { 0 };
    u32 capacity { 0 };
    IAllocator* pAlloc { nullptr };

    inline ResizableArray(IAllocator* _pAlloc = &g_Allocator);

    void Free();

    template<typename F>
    void Free(F&& freeElement);

    void Resize(u32 desiredCount);

    void Reserve(u32 desiredCapacity);

    void PushBack(const Type& value);

    void PopBack();

    Type& operator[](usize i);

    const Type& operator[](usize i) const;

    void Erase(usize index);

    void EraseUnsorted(usize index);

    void Insert(usize index, const Type& value);

    Type* Find(const Type& value);

    Type* begin();

    Type* end();

    const Type* begin() const;

    const Type* end() const;

    u32 IndexFromPointer(const Type* ptr) const;

    bool Validate() const;

    u32 GrowCapacity(u32 atLeastSize) const;
};
