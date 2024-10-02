// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

#include "types.h"
#include "memory.h"

// String Class
// --------------------
// NOT null terminated string
// The functions in this struct guarantee to do zero memory operations
// and is fully POD, trivially copyable

struct String {
	char* pData = nullptr;
    size length = 0;

    // TODOs
    // [ ] contains/nocase
    // [ ] Tof32/ToInt
    // [ ] trimRight/Left

    String() {}

    String(const char* str);

    void operator=(const char* str);

    bool operator==(const String& other) const;

    bool operator==(const char* other) const;

    bool operator!=(const String& other) const;

    bool operator!=(const char* other) const;

    String SubStr(size start, size len = -1);
};

String CopyCString(const byte* string, Arena* pArena);

String CopyCStringRange(byte* start, byte* end, Arena* pArena);

String CopyString(String& string, Arena* pArena);

String AllocString(usize length, Arena* pArena);

