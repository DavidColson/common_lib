#pragma once

#include "memory.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// String Class
// --------------------
// Null terminated string
// The functions in this struct guarantee to do zero memory operations
// and is fully POD, trivially copyable
// TODO: Documentation

struct String {
    char* pData = nullptr;
    size_t length = 0;

    // TODOs
    // [ ] contains/nocase
    // [ ] create substring
    // [ ] Tofloat/ToInt
    // [ ] trimRight/Left

    String() {}

    String(const char* str) {
        pData = const_cast<char*>(str);
        length = strlen(str);
    }

    void operator=(const char* str) {
        pData = const_cast<char*>(str);
        length = strlen(str);
    }

    bool operator==(const String& other) const {
        if (length != other.length)
            return false;
        char* s1 = pData;
        char* s2 = other.pData;
        size_t count = 0;
        while (count < length) {
            count++;
            if (*s1 != *s2)
                return false;
            s1++;
            s2++;
        }
        return true;
    }

    bool operator==(const char* other) const {
        String str(other);
        return operator==(str);
    }

    bool operator!=(const String& other) const {
        return !operator==(other);
    }

    bool operator!=(const char* other) const {
        return !operator==(other);
    }

    String SubStr(size_t start, size_t len = -1) {
        String result;
        result.pData = pData + start;

        if (len == (size_t)-1)
            result.length = length - start;
        else
            result.length = len;
        return result;
    }
};

String CopyCString(const char* string, IAllocator* pAlloc = &gAllocator);

String CopyCStringRange(char* start, char* end, IAllocator* pAlloc = &gAllocator);

String CopyString(String& string, IAllocator* pAlloc = &gAllocator);

String AllocString(size_t length, IAllocator* pAlloc = &gAllocator);

void FreeString(String& string, IAllocator* pAlloc = &gAllocator);
