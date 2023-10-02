// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

#include "memory.h"

// String Builder
// ---------------------
// A dynamic char buffer used to construct strings
// Allocates memory on resize, must be freed manually
// TODO: Documentation

struct String;
struct IAllocator;
typedef char* va_list;

struct StringBuilder {
    char* pData = nullptr;
    size_t length = 0;
    size_t capacity = 0;
    IAllocator* pAlloc { nullptr };

    StringBuilder(IAllocator* _pAlloc = &g_Allocator);

    void AppendChars(const char* str, size_t len);

    void Append(const char* str);

    void Append(String str);

    void AppendFormatInternal(const char* format, va_list args);

    void AppendFormat(const char* format, ...);

    String CreateString(bool reset = true, IAllocator* _pAlloc = &g_Allocator);

    String ToExistingString(bool reset, String& destination);

    void Reset();

    void Reserve(size_t desiredCapacity);

    size_t GrowCapacity(size_t atLeastSize) const;
};
