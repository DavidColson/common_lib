#pragma once

#include "memory.h"

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#ifdef _DEBUG
#define DEBUG_CHECK(expression) if (!(expression)) __debugbreak();
#else
#define DEBUG_CHECK(expression)
#endif

// Dynamic String Class
// --------------------
// Null terminated string
// The functions in this struct guarantee to do zero memory operations
// and is fully POD, trivially copyable
// TODO: Documentation

struct String {
	char* pData = nullptr;
	size_t length = 0;
	
	// TODOs
	// [ ] contains/nocase
	// [ ] create substring
	// [ ] Tofloat/ToInt
	// [ ] trimRight/Left

	String() {}
	
	String(const char* str) {
		pData = const_cast<char*>(str);
		length = strlen(str);
	}

	void operator=(const char* str) {
		pData = const_cast<char*>(str);
		length = strlen(str);
	}

	bool operator==(const String& other) const {
		if (length != other.length) return false;
		char* s1 = pData;
		char* s2 = other.pData;
		size_t count = 0;
		while (count < length) {
			count++;
			if (*s1 != *s2) return false;
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
};

String CopyCString(const char* string, IAllocator* pAlloc = &gAllocator);

String CopyCStringRange(char* start, char* end, IAllocator* pAlloc = &gAllocator);

String CopyString(String& string, IAllocator* pAlloc = &gAllocator);

String AllocString(size_t length, IAllocator* pAlloc = &gAllocator);

void FreeString(String& string, IAllocator* pAlloc = &gAllocator);
