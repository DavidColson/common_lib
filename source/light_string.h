// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

#include "memory.h"

// String Class
// --------------------
// Null terminated string
// The functions in this struct guarantee to do zero memory operations
// and is fully POD, trivially copyable
// TODO: Documentation

struct String {
    char* m_pData = nullptr;
    size_t m_length = 0;

    // TODOs
    // [ ] contains/nocase
    // [ ] create substring
    // [ ] Tofloat/ToInt
    // [ ] trimRight/Left

    String() {}

    String(const char* str);

    void operator=(const char* str);

    bool operator==(const String& other) const;

    bool operator==(const char* other) const;

    bool operator!=(const String& other) const;

    bool operator!=(const char* other) const;

    String SubStr(size_t start, size_t len = -1);
};

String CopyCString(const char* string, IAllocator* pAlloc = &g_Allocator);

String CopyCStringRange(char* start, char* end, IAllocator* pAlloc = &g_Allocator);

String CopyString(String& string, IAllocator* pAlloc = &g_Allocator);

String AllocString(size_t length, IAllocator* pAlloc = &g_Allocator);

void FreeString(String& string, IAllocator* pAlloc = &g_Allocator);
