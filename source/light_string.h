// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

#include "types.h"
#include "memory.h"

// String Class
// --------------------
// Null terminated string
// The functions in this struct guarantee to do zero memory operations
// and is fully POD, trivially copyable
// TODO: Documentation

struct String {
    byte* pData = nullptr;
    usize length = 0;

    // TODOs
    // [ ] contains/nocase
    // [ ] create substring
    // [ ] Tof32/ToInt
    // [ ] trimRight/Left

    String() {}

    String(const byte* str);

    void operator=(const byte* str);

    bool operator==(const String& other) const;

    bool operator==(const byte* other) const;

    bool operator!=(const String& other) const;

    bool operator!=(const byte* other) const;

    String SubStr(usize start, usize len = -1);
};

String CopyCString(const byte* string, IAllocator* pAlloc = &g_Allocator);

String CopyCStringRange(byte* start, byte* end, IAllocator* pAlloc = &g_Allocator);

String CopyString(String& string, IAllocator* pAlloc = &g_Allocator);

String AllocString(usize length, IAllocator* pAlloc = &g_Allocator);

void FreeString(String& string, IAllocator* pAlloc = &g_Allocator);
