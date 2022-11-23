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
// Null terminated String Type

template<typename Type, typename AllocatorType = Allocator>
struct BaseString {
	Type* pData = nullptr;
	uint32_t length = 0;
	uint32_t capacity = 0;
	AllocatorType allocator;
	
	BaseString() {}

	BaseString(const Type* str) {
		length = (uint32_t)strlen(str);
		Reserve(GrowCapacity(length + 1));
		memcpy(pData, str, length + 1);
	}

	BaseString(const BaseString& toCopy) {
		length = toCopy.length;
		Reserve(toCopy.capacity);
		memcpy(pData, toCopy.pData, length + 1);
	}

	~BaseString() {
		if (pData) allocator.Free(pData);
	}

	BaseString& operator=(const Type* str) {
		length = (uint32_t)strlen(str);
		Reserve(GrowCapacity(length + 1));
		memcpy(pData, str, length + 1);
		return *this;
	}

	BaseString& operator=(const BaseString& toCopy) {
		length = toCopy.length;
		Reserve(toCopy.capacity);
		memcpy(pData, toCopy.pData, length + 1);
		return *this;
	}

	void AppendChars(const Type* str, uint32_t len) {
		Reserve(GrowCapacity(length + len + 1));
		memcpy(pData + length, str, len);
		length += len;
		pData[length] = '\0';
	}

	void Append(const Type* str) {
		uint32_t addedLength = (uint32_t)strlen(str);
		AppendChars(str, addedLength);
	}

	void AppendFormatInternal(const Type* format, va_list args) {
		va_list argsCopy;
		va_copy(argsCopy, args);

		int addedLength = vsnprintf(nullptr, 0, format, args);
		if (addedLength <= 0) {
			va_end(argsCopy);
			return;
		}

		Reserve(GrowCapacity(length + addedLength + 1));
		vsnprintf(pData + length, addedLength + 1, format, args);
		va_end(argsCopy);
		length += addedLength;
	}

	void AppendFormat(const Type* format, ...) {
		va_list args;
		va_start(args, format);
		AppendFormatInternal(format, args);
		va_end(args);
	}

	void Clear() {
		SYS_FREE(pData);
		pData = nullptr;
		length = 0;
		capacity = 0;
	}

	bool operator==(const Type* other) {
		if (strcmp(pData, other) == 0)
			return true;
		return false;
	}

	bool operator!=(const Type* other) {
		return !operator==(other);
	}

	bool operator==(const BaseString& other) {
		return operator==(other.pData);
	}

	bool operator!=(const BaseString& other) {
		return !operator==(other.pData);
	}

	Type* begin() {
		return pData;
	}

	Type* end() {
		return pData + length;
	}

	const Type* begin() const{
		return pData;
	}

	const Type* end() const {
		return pData + length;
	}

	void Reserve(uint32_t desiredCapacity) {
		if (capacity >= desiredCapacity) return;
		pData = (Type*)allocator.Reallocate(pData, desiredCapacity * sizeof(Type));
		capacity = desiredCapacity;
	}

	uint32_t GrowCapacity(uint32_t atLeastSize) const {
		// if we're big enough already, don't grow, otherwise double, 
		// and if that's not enough just use atLeastSize
		if (capacity > atLeastSize) return capacity;
		uint32_t newCapacity = capacity ? capacity * 2 : 8;
		return newCapacity > atLeastSize ? newCapacity : atLeastSize;
	}
};

typedef BaseString<char> String;
typedef BaseString<wchar_t> WString;
