// Copyright 2020-2022 David Colson. All rights reserved.

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
    i64 count = 0;
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

char String::operator[](i64 index) const {
	return pData[index];
}

// ***********************************************************************

String String::SubStr(i64 start, i64 len) {
    String result;
    result.pData = pData + start;

	// What the fuck is this?
    if (len == (u64)-1)
        result.length = length - start;
    else
        result.length = len;
    return result;
}

// ***********************************************************************

i64 String::Find(String substr) {
	if (substr.length == 0) return -1;

	for(i64 i = 0; i < length; i++) {
		i64 j = i; // index into self
		i64 k = 0; // index into substr
		while(j < length && k < substr.length && pData[j] == substr.pData[k]) {
			j++;
			k++;
		}

		// found
		if (k==substr.length) return j;
	}
	return -1;
}

// ***********************************************************************

String CopyCString(const char* string, Arena* pArena) {
    String s;
    u64 len = strlen(string);
	s.pData = New(pArena, char, len + 1);
    s.length = len;
    memcpy(s.pData, string, (len + 1) * sizeof(char));
    return s;
}

// ***********************************************************************

String CopyCStringRange(char* start, char* end, Arena* pArena) {
    String s;
    u64 len = end - start;
	s.pData = New(pArena, char, len + 1);
    s.length = len;
    memcpy(s.pData, start, len * sizeof(char));
    s.pData[s.length] = 0;
    return s;
}

// ***********************************************************************

String CopyString(String& string, Arena* pArena) {
    String s;
	s.pData = New(pArena, char, string.length + 1);
    s.length = string.length;
    memcpy(s.pData, string.pData, string.length * sizeof(char));
    s.pData[string.length] = 0;
    return s;
}

// ***********************************************************************

String AllocString(u64 length, Arena* pArena) {
    String s;
	s.pData = New(pArena, char, length + 1);
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
