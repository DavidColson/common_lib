// Copyright 2020-2022 David Colson. All rights reserved.

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

bool StrCmp(const String& lhs, const String& rhs, StrCmpFlags flags) {
    if (lhs.length == rhs.length && flags == 0) {
		return memcmp(lhs.pData, rhs.pData, lhs.length) == 0;
	}
	else if (lhs.length == rhs.length) {
		char* s1 = lhs.pData;
		char* s2 = rhs.pData;
		i64 count = 0;
		i64 lenToScan = min(lhs.length, rhs.length);
		while (count < lhs.length) {
			count++;
			char a = *s1;
			char b = *s2;
			if (flags & CaseInsensitive) {
				a = CharToUpper(a);
				b = CharToUpper(b);
			}
			if (flags & SlashInsensitive) {
				a = SlashNormalize(a);
				b = SlashNormalize(b);
			}
			if (a != b)
				return false;
			s1++;
			s2++;
		}
		return true;
	}
	return false;
}

// ***********************************************************************

bool operator==(const String& lhs, const String& rhs) {
	return StrCmp(lhs, rhs);
}

// ***********************************************************************

bool operator==(const String& lhs, const char* rhs) {
    return StrCmp(lhs, String(rhs));
}

// ***********************************************************************

bool operator!=(const String& lhs, const String& rhs) {
    return !StrCmp(lhs, rhs);
}

// ***********************************************************************

bool StartsWith(String str, String prefix, StrCmpFlags flags) {
	String start = Prefix(str, prefix.length);
	return StrCmp(start, prefix, flags);
}

// ***********************************************************************

bool EndsWith(String str, String postfix, StrCmpFlags flags) {
	String end = Postfix(str, postfix.length);
	return StrCmp(end, postfix, flags);
}

// ***********************************************************************

bool operator!=(const String& lhs, const char* rhs) {
    return !StrCmp(lhs, rhs);
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

String Prefix(String str, i64 n) {
	String result;
	n = min(str.length, n);
	result.pData = str.pData;
	result.length = n;
	return result;
}

// ***********************************************************************

String Postfix(String str, i64 n) {
	String result;
	n = min(str.length, n);
	result.pData = str.pData + (str.length - n);
	result.length = n;
	return result;
}

// ***********************************************************************

String ChopLeft(String str, i64 n) {
	String result = str;
	n = min(str.length, n);
	result.pData += n;
	result.length -= n;
	return result;
}

// ***********************************************************************

String ChopRight(String str, i64 n) {
	String result = str;
	n = min(str.length, n);
	result.length -= n;
	return result;
}

// ***********************************************************************

ResizableArray<String> Split(Arena* pArena, String str, String splitChars) {
	ResizableArray<String> result;
	result.pArena = pArena;

	i64 lastSeparation = 0;
	for(i64 i = 0; i < str.length; i++) {
		for (i64 j = 0; j < splitChars.length; j++) {
			if (str[i] == splitChars[j]) {
				result.PushBack(SubStr(str, lastSeparation, i - lastSeparation));
				lastSeparation = i+1;
				continue;
			}
		}
	}
	result.PushBack(SubStr(str, lastSeparation, str.length - lastSeparation));
	return result;
}


// ***********************************************************************

String TakeAfterLastSlash(String str) {
	for (int i=str.length; i >= 0; i--) {
		if (str[i] == '/' || str[i] == '\\') {
			return SubStr(str, i+1, str.length-i-1); 
		}
	}
	return str;
}

// ***********************************************************************

String TakeBeforeLastSlash(String str) {
	for (int i=str.length; i >= 0; i--) {
		if (str[i] == '/' || str[i] == '\\') {
			return SubStr(str, 0, i); 
		}
	}
	return str;
}

// ***********************************************************************

String TakeAfterLastDot(String str) {
	for (int i=str.length; i >= 0; i--) {
		if (str[i] == '.') {
			return SubStr(str, i+1, str.length-i-1);
		}
	}
	return str;
}

// ***********************************************************************

String TakeBeforeLastDot(String str) {
	for (int i=str.length; i >= 0; i--) {
		if (str[i] == '.') {
			return SubStr(str, 0, i); 
		}
	}
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

String Join(Arena* pArena, ResizableArray<String> parts, String separator) {
	StringBuilder builder(g_pArenaFrame);
	for (int i=0; i<parts.count; i++) {
		builder.Append(parts[i]);
		if (i<parts.count-1) {
			builder.Append(separator);
		}
	}
	return builder.CreateString(pArena);
}

// ***********************************************************************

char CharToUpper(char c) {
	if ('a' <= c && c <= 'z') {
		return c += ('A' - 'a');
	}
	return c;
}

// ***********************************************************************

char CharToLower(char c) {
	if ('A' <= c && c <= 'Z') {
		return c += ('a' - 'A');
	}
	return c;
}

// ***********************************************************************

char SlashNormalize(char c) {
	if (c == '\\' || c == '/') {
		return '/';
	}
	return c;
}
