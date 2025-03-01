// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

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
    u64 length { 0 };
    u64 capacity { 0 };
    Arena* pArena { nullptr };

    StringBuilder(Arena* pArena);

    void AppendChars(const char* str, u64 len);

    void Append(const char* str);

    void Append(String str);

    void AppendFormatInternal(const char* format, va_list args);

    void AppendFormat(const char* format, ...);

    String CreateString(Arena* pArena, bool reset = true);

    String ToExistingString(bool reset, String& destination);

    void Reset();

    void Reserve(u64 desiredCapacity);

    u64 GrowCapacity(u64 atLeastSize) const;
};
