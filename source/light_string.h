// Copyright 2020-2022 David Colson. All rights reserved.

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
    char* m_pData = nullptr;
    size_t m_length = 0;

    // TODOs
    // [ ] contains/nocase
    // [ ] create substring
    // [ ] Tofloat/ToInt
    // [ ] trimRight/Left

    String() {}

    String(const char* str) {
        m_pData = const_cast<char*>(str);
        m_length = strlen(str);
    }

    void operator=(const char* str) {
        m_pData = const_cast<char*>(str);
        m_length = strlen(str);
    }

    bool operator==(const String& other) const {
        if (m_length != other.m_length)
            return false;
        char* s1 = m_pData;
        char* s2 = other.m_pData;
        size_t count = 0;
        while (count < m_length) {
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
        result.m_pData = m_pData + start;

        if (len == (size_t)-1)
            result.m_length = m_length - start;
        else
            result.m_length = len;
        return result;
    }
};

String CopyCString(const char* string, IAllocator* pAlloc = &g_Allocator);

String CopyCStringRange(char* start, char* end, IAllocator* pAlloc = &g_Allocator);

String CopyString(String& string, IAllocator* pAlloc = &g_Allocator);

String AllocString(size_t length, IAllocator* pAlloc = &g_Allocator);

void FreeString(String& string, IAllocator* pAlloc = &g_Allocator);
