// Copyright 2020-2022 David Colson. All rights reserved.

#include "light_string.h"

#include <string.h>

// ***********************************************************************

String::String(const char* str) {
    pData = const_cast<char*>(str);
    length = strlen(str);
}

// ***********************************************************************

void String::operator=(const char* str) {
    pData = const_cast<char*>(str);
    length = strlen(str);
}

// ***********************************************************************

bool String::operator==(const String& other) const {
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

// ***********************************************************************

bool String::operator==(const char* other) const {
    String str(other);
    return operator==(str);
}

// ***********************************************************************

bool String::operator!=(const String& other) const {
    return !operator==(other);
}

// ***********************************************************************

bool String::operator!=(const char* other) const {
    return !operator==(other);
}

// ***********************************************************************

String String::SubStr(size_t start, size_t len) {
    String result;
    result.pData = pData + start;

    if (len == (size_t)-1)
        result.length = length - start;
    else
        result.length = len;
    return result;
}

// ***********************************************************************

String CopyCString(const char* string, IAllocator* pAlloc) {
    String s;
    size_t len = strlen(string);
    s.pData = (char*)pAlloc->Allocate((len + 1) * sizeof(char));
    s.length = len;
    memcpy(s.pData, string, (len + 1) * sizeof(char));
    return s;
}

// ***********************************************************************

String CopyCStringRange(char* start, char* end, IAllocator* pAlloc) {
    String s;
    size_t len = end - start;
    s.pData = (char*)pAlloc->Allocate((len + 1) * sizeof(char));
    s.length = len;
    memcpy(s.pData, start, len * sizeof(char));
    s.pData[s.length] = 0;
    return s;
}

// ***********************************************************************

String CopyString(String& string, IAllocator* pAlloc) {
    String s;
    s.pData = (char*)pAlloc->Allocate((string.length + 1) * sizeof(char));
    s.length = string.length;
    memcpy(s.pData, string.pData, string.length * sizeof(char));
    s.pData[string.length] = 0;
    return s;
}

// ***********************************************************************

String AllocString(size_t length, IAllocator* pAlloc) {
    String s;
    s.pData = (char*)pAlloc->Allocate(length * sizeof(char));
    s.length = length;
    return s;
}

// ***********************************************************************

void FreeString(String& string, IAllocator* pAlloc) {
    if (string.pData) {
        pAlloc->Free(string.pData);
        string.pData = nullptr;
        string.length = 0;
    }
}
