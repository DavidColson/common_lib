// Copyright 2020-2022 David Colson. All rights reserved.

#include "light_string.h"

#include <string.h>

// ***********************************************************************

String::String(const byte* str) {
    pData = const_cast<byte*>(str);
    length = strlen(str);
}

// ***********************************************************************

void String::operator=(const byte* str) {
    pData = const_cast<byte*>(str);
    length = strlen(str);
}

// ***********************************************************************

bool String::operator==(const String& other) const {
    if (length != other.length)
        return false;
    byte* s1 = pData;
    byte* s2 = other.pData;
    size count = 0;
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

bool String::operator==(const byte* other) const {
    String str(other);
    return operator==(str);
}

// ***********************************************************************

bool String::operator!=(const String& other) const {
    return !operator==(other);
}

// ***********************************************************************

bool String::operator!=(const byte* other) const {
    return !operator==(other);
}

// ***********************************************************************

String String::SubStr(size start, size len) {
    String result;
    result.pData = pData + start;

	// What the fuck is this?
    if (len == (usize)-1)
        result.length = length - start;
    else
        result.length = len;
    return result;
}

// ***********************************************************************

String CopyCString(const byte* string, Arena* pArena) {
    String s;
    usize len = strlen(string);
	s.pData = New(pArena, byte, len + 1);
    s.length = len;
    memcpy(s.pData, string, (len + 1) * sizeof(byte));
    return s;
}

// ***********************************************************************

String CopyCStringRange(byte* start, byte* end, Arena* pArena) {
    String s;
    usize len = end - start;
	s.pData = New(pArena, byte, len + 1);
    s.length = len;
    memcpy(s.pData, start, len * sizeof(byte));
    s.pData[s.length] = 0;
    return s;
}

// ***********************************************************************

String CopyString(String& string, Arena* pArena) {
    String s;
	s.pData = New(pArena, byte, string.length + 1);
    s.length = string.length;
    memcpy(s.pData, string.pData, string.length * sizeof(byte));
    s.pData[string.length] = 0;
    return s;
}

// ***********************************************************************

String AllocString(usize length, Arena* pArena) {
    String s;
	s.pData = New(pArena, byte, length + 1);
    s.length = length;
    return s;
}

// ***********************************************************************

void FreeString(String& string) {
    if (string.pData) {
		RawFree(string.pData);
        string.pData = nullptr;
        string.length = 0;
    }
}
