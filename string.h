#pragma once

#include <stdint.h>

// Dynamic String Class
// --------------------
// ... TODO Documentation etc
// Null terminated string Type

struct String {
	char* pData = nullptr;
	uint32_t length = 0;
	uint32_t capacity = 0;
	
	String() {}

	String(const char* str);

	String(const String& toCopy);

	~String();

	String& operator=(const char* str);

	String& operator=(const String& toCopy);

	void AppendChars(const char* str, uint32_t len);

	void Append(const char* str);

	void AppendFormatInternal(const char* format, va_list args);

	void AppendFormat(const char* format, ...);

	void Clear();

	bool operator==(const char* other);

	bool operator!=(const char* other);

	bool operator==(const String& other);

	bool operator!=(const String& other);

	char* begin();

	char* end();

	const char* begin() const;

	const char* end() const;

	void Reserve(uint32_t desiredCapacity);

	uint32_t GrowCapacity(uint32_t atLeastSize) const;
};