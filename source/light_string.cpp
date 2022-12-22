// Copyright 2020-2022 David Colson. All rights reserved.

#include "light_string.h"

#include <string.h>

// ***********************************************************************

String::String(const char* str) {
    m_pData = const_cast<char*>(str);
    m_length = strlen(str);
}

// ***********************************************************************

void String::operator=(const char* str) {
    m_pData = const_cast<char*>(str);
    m_length = strlen(str);
}

// ***********************************************************************

bool String::operator==(const String& other) const {
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
    result.m_pData = m_pData + start;

    if (len == (size_t)-1)
        result.m_length = m_length - start;
    else
        result.m_length = len;
    return result;
}

// ***********************************************************************

String CopyCString(const char* string, IAllocator* pAlloc) {
    String s;
    size_t len = strlen(string);
    s.m_pData = (char*)pAlloc->Allocate((len + 1) * sizeof(char));
    s.m_length = len;
    memcpy(s.m_pData, string, (len + 1) * sizeof(char));
    return s;
}

// ***********************************************************************

String CopyCStringRange(char* start, char* end, IAllocator* pAlloc) {
    String s;
    size_t len = end - start;
    s.m_pData = (char*)pAlloc->Allocate((len + 1) * sizeof(char));
    s.m_length = len;
    memcpy(s.m_pData, start, len * sizeof(char));
    s.m_pData[s.m_length] = 0;
    return s;
}

// ***********************************************************************

String CopyString(String& string, IAllocator* pAlloc) {
    String s;
    s.m_pData = (char*)pAlloc->Allocate((string.m_length + 1) * sizeof(char));
    s.m_length = string.m_length;
    memcpy(s.m_pData, string.m_pData, string.m_length * sizeof(char));
    s.m_pData[string.m_length] = 0;
    return s;
}

// ***********************************************************************

String AllocString(size_t length, IAllocator* pAlloc) {
    String s;
    s.m_pData = (char*)pAlloc->Allocate(length * sizeof(char));
    s.m_length = length;
    return s;
}

// ***********************************************************************

void FreeString(String& string, IAllocator* pAlloc) {
    pAlloc->Free(string.m_pData);
    string.m_pData = nullptr;
    string.m_length = 0;
}
