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
    char* pData { nullptr };
    usize length { 0 };
    usize capacity { 0 };
    Arena* pArena { nullptr };

    StringBuilder(Arena* pArena);

    void AppendChars(const char* str, usize len);

    void Append(const char* str);

    void Append(String str);

    void AppendFormatInternal(const char* format, va_list args);

    void AppendFormat(const char* format, ...);

    String CreateString(Arena* pArena, bool reset = true);

    String ToExistingString(bool reset, String& destination);

    void Reset();

    void Reserve(usize desiredCapacity);

    usize GrowCapacity(usize atLeastSize) const;
};
