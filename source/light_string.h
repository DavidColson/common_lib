// Copyright David Colson. All rights reserved.

#pragma once

// String Class
// --------------------
// NOT null terminated string
// vast majority of these functions do zero memory allocation
// functions that take arenas are the only ones that do allocations

struct String {
	char* pData = nullptr;
    i64 length = 0;

	// usuall we don't like to have constructors in this codebase, but this is 
	// more convenient than wrapping string literals everywhere with some macro
	String() {}
	String(const char* str) {pData=(char*)str; length=strlen(str);}

	// convenience access to pData
    char operator[](i64 index) const { return pData[index]; }
    char& operator[](i64 index) { return pData[index]; }
};

// Construction/Copying

String CopyCString(const char* string, Arena* pArena);

String CopyCStringRange(char* start, char* end, Arena* pArena);

String CopyString(String& string, Arena* pArena);

String AllocString(u64 length, Arena* pArena);
	
// Comparison

enum StrCmpFlags : u8 {
	CaseInsensitive = 1 << 1,
	SlashInsensitive = 1 << 2
};

bool StrCmp(const String& lhs, const String& rhs, StrCmpFlags flags = StrCmpFlags(0));

bool operator==(const String& lhs, const String& rhs);

bool operator==(const String& lhs, const char* rhs);

bool operator!=(const String& lhs, const String& rhs);

bool operator!=(const String& lhs, const char* rhs);

bool StartsWith(String str, String prefix, StrCmpFlags flags = StrCmpFlags(0));

bool EndsWith(String str, String postfix, StrCmpFlags flags = StrCmpFlags(0));


// Slicing

String SubStr(String str, i64 start, i64 len = -1);

String Prefix(String str, i64 n);

String Postfix(String str, i64 n);

String ChopLeft(String str, i64 n);

String ChopRight(String str, i64 n);

ResizableArray<String> Split(Arena* pArena, String str, String splitChars);

// Paths

String TakeAfterLastSlash(String str);

String TakeBeforeLastSlash(String str);

String TakeAfterLastDot(String str);

String TakeBeforeLastDot(String str);

void NormalizePath(String path); // modifies given str

// Misc

// Returns the index substr was found at, otherwise length of str
i64 Find(String str, String substr);

String Join(Arena* pArena, ResizableArray<String> parts, String separator);

char CharToUpper(char c);

char CharToLower(char c);

char SlashNormalize(char c);

