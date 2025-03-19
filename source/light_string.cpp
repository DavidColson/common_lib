// Copyright 2020-2022 David Colson. All rights reserved.

// ***********************************************************************

bool operator==(const String& lhs, const String& rhs) {
    if (lhs.length != rhs.length)
        return false;
    char* s1 = lhs.pData;
    char* s2 = rhs.pData;
    i64 count = 0;
    while (count < lhs.length) {
        count++;
        if (*s1 != *s2)
            return false;
        s1++;
        s2++;
    }
    return true;
}

// ***********************************************************************

bool operator==(const String& lhs, const char* rhs) {
    return operator==(lhs, String(rhs));
}

// ***********************************************************************

bool operator!=(const String& lhs, const String& rhs) {
    return !operator==(lhs, rhs);
}

// ***********************************************************************

bool operator!=(const String& lhs, const char* rhs) {
    return !operator==(lhs, rhs);
}

// ***********************************************************************

String SubStr(String str, i64 start, i64 len) {
    String result;
    result.pData = str.pData + start;

	// What the fuck is this?
    if (len == (u64)-1)
        result.length = str.length - start;
    else
        result.length = len;
    return result;
}

// ***********************************************************************

i64 Find(String str, String substr) {
	if (substr.length == 0) return -1;

	for(i64 i = 0; i < str.length; i++) {
		i64 j = i; // index into self
		i64 k = 0; // index into substr
		while(j < str.length && k < substr.length && str.pData[j] == substr.pData[k]) {
			j++;
			k++;
		}

		// found
		if (k==substr.length) return i;
	}
	return str.length;
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
