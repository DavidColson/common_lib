// Copyright 2020-2022 David Colson. All rights reserved.

#include "light_string.h"

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
