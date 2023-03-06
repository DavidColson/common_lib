// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

#include "memory.h"

#include <stdint.h>

// Resizable Array Structure
// -----------------------
// Must be freed manually
// ... todo documentation, examples, reasoning etc

template<typename Type>
struct ResizableArray {
    Type* pData { nullptr };
    uint32_t count { 0 };
    uint32_t capacity { 0 };
    IAllocator* pAlloc { nullptr };

    inline ResizableArray(IAllocator* _pAlloc = &g_Allocator);

    void Free();

    template<typename F>
    void Free(F&& freeElement);

    void Resize(uint32_t desiredCount);

    void Reserve(uint32_t desiredCapacity);

    void PushBack(const Type& value);

    void PopBack();

    Type& operator[](size_t i);

    const Type& operator[](size_t i) const;

    void Erase(size_t index);

    void EraseUnsorted(size_t index);

    void Insert(size_t index, const Type& value);

    Type* Find(const Type& value);

    Type* begin();

    Type* end();

    const Type* begin() const;

    const Type* end() const;

    uint32_t IndexFromPointer(const Type* ptr) const;

    bool Validate() const;

    uint32_t GrowCapacity(uint32_t atLeastSize) const;
};
