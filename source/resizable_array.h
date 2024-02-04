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
    size count { 0 };
    size capacity { 0 };
    IAllocator* pAlloc { nullptr };

    inline ResizableArray(IAllocator* _pAlloc = &g_Allocator);

    void Free();

    template<typename F>
    void Free(F&& freeElement);

    void Resize(size desiredCount);

    void Reserve(size desiredCapacity);

    void PushBack(const Type& value);

    void PopBack();

    Type& operator[](size i);

    const Type& operator[](size i) const;

    void Erase(size index);

    void EraseUnsorted(size index);

    void Insert(size index, const Type& value);

    Type* Find(const Type& value);

    Type* begin();

    Type* end();

    const Type* begin() const;

    const Type* end() const;

    size IndexFromPointer(const Type* ptr) const;

    bool Validate() const;

    size GrowCapacity(size atLeastSize) const;
};
