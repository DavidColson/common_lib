// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

#include "types.h"
#include "memory.h"

// Resizable Array Structure
// -----------------------
// ... todo documentation, examples, reasoning etc

template<typename Type>
struct ResizableArray {
    Type* pData{nullptr};
    size count{0};
    size capacity{0};
	Arena* pArena{nullptr};

    ResizableArray(Arena* _pArena = nullptr);

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
