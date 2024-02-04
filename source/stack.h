// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

#include "resizable_array.h"

// Stack Structure
// -----------------------
// Must be freed manually
// Can use negative indices (i.e. -1 is top of stack, -count is bottom of stack)
// ... todo documentation, examples, reasoning etc

template<typename T>
struct Stack {
    ResizableArray<T> array;

    Stack(IAllocator* pAlloc = &g_Allocator);

    void Free();

    template<typename F>
    void Free(F&& freeElement);

    void Resize(u64 desiredCount);

    void Reserve(u64 desiredCapacity);

    void Push(const T& value);

    T Pop();

    T& Top();

    T& operator[](i64 i);

    const T& operator[](i64 i) const;
};
