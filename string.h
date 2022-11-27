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
// ... TODO Documentation etc
// NOT null terminated String Type

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

template<typename AllocatorType = Allocator>
String CopyCString(const char* string, bool withTerminator=false, AllocatorType allocator = Allocator()) {
	String s;
	size_t len = strlen(string);
	s.pData = (char*)allocator.Allocate((len+1) * sizeof(char)); // TODO don't copy the terminator when class is fully non-null terminated
	s.length = len;
	memcpy(s.pData, string, (len+1) * sizeof(char));
	return s;
}

template<typename AllocatorType = Allocator>
String CopyString(String& string, bool withTerminator=false, AllocatorType allocator = Allocator()) {
	String s;
	s.pData = (char*)allocator.Allocate((string.length+1) * sizeof(char)); // TODO don't copy the terminator when class is fully non-null terminated
	s.length = string.length;
	memcpy(s.pData, string.pData, string.length * sizeof(char));
	s.pData[string.length] = 0;
	return s;
}

template<typename AllocatorType = Allocator>
String AllocString(size_t length, AllocatorType allocator = Allocator()) {
	String s;
	s.pData = (char*)allocator.Allocate(length * sizeof(char));
	s.length = length;
	return s;
}

template<typename AllocatorType = Allocator>
void FreeString(String& string, AllocatorType allocator = Allocator()) {
	allocator.Free(string.pData);
	string.pData = nullptr;
	string.length = 0;
}
