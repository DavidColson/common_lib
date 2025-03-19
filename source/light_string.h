// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

// String Class
// --------------------
// NOT null terminated string
// The functions in this struct guarantee to do zero memory operations
// and is fully POD, trivially copyable

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

bool operator==(const String& lhs, const String& rhs);

bool operator==(const String& lhs, const char* rhs);

bool operator!=(const String& lhs, const String& rhs);

bool operator!=(const String& lhs, const char* rhs);

// Searching

// Returns the index substr was found at, otherwise length of str
i64 Find(String str, String substr);

String SubStr(String str, i64 start, i64 len = -1);


