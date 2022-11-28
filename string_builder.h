#pragma once

#include "string.h"

// String Builder
// ---------------------
// A dynamic char buffer used to construct strings
// Allocates memory on resize, must be freed manually
// TODO: Documentation


template<typename AllocatorType = Allocator>
struct StringBuilder {
	char* pData = nullptr;
	uint32_t length = 0;
	uint32_t capacity = 0;
	AllocatorType allocator;

	StringBuilder() {}

	StringBuilder(AllocatorType _allocator) {
		allocator = _allocator;
	}

	void AppendChars(const char* str, uint32_t len) {
		Reserve(GrowCapacity(length + len));
		memcpy(pData + length, str, len);
		length += len;
	}

	void Append(const char* str) {
		uint32_t addedLength = (uint32_t)strlen(str);
		AppendChars(str, addedLength);
	}

	void Append(String str) {
		AppendChars(str.pData, str.length);
	}

	void AppendFormatInternal(const char* format, va_list args) {
		va_list argsCopy;
		va_copy(argsCopy, args);

		int addedLength = vsnprintf(nullptr, 0, format, args);
		if (addedLength <= 0) {
			va_end(argsCopy);
			return;
		}

		Reserve(GrowCapacity(length + addedLength));
		vsnprintf(pData + length, addedLength + 1, format, args);
		va_end(argsCopy);
		length += addedLength;
	}

	void AppendFormat(const char* format, ...) {
		va_list args;
		va_start(args, format);
		AppendFormatInternal(format, args);
		va_end(args);
	}

	String CreateString(bool withTerminator = false, bool reset = true) {
		String output = AllocString(length+1, allocator);
		memcpy(output.pData, pData, (length+1) * sizeof(char));
		output.pData[length] = 0;
		output.length = length;

		if (reset) Reset();
		return output;
	}

	void Reset() {
		allocator.Free(pData);
		pData = nullptr;
		length = 0;
		capacity = 0;
	}

	void Reserve(uint32_t desiredCapacity) {
		if (capacity >= desiredCapacity) return;
		pData = (char*)allocator.Reallocate(pData, desiredCapacity * sizeof(char), capacity * sizeof(char));
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