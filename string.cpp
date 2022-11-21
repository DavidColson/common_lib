#include "string.h"

#include "sys_alloc.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

String::String(const char* str) {
	length = (uint32_t)strlen(str);
	Reserve(GrowCapacity(length + 1));
	memcpy(pData, str, length + 1);
}

String::String(const String& toCopy) {
	length = toCopy.length;
	Reserve(toCopy.capacity);
	memcpy(pData, toCopy.pData, length);
}

String::~String() {
	if (pData) SYS_FREE(pData);
}

String& String::operator=(const char* str) {
	length = (uint32_t)strlen(str);
	Reserve(GrowCapacity(length + 1));
	memcpy(pData, str, length + 1);
	return *this;
}

String& String::operator=(const String& toCopy) {
	length = toCopy.length;
	Reserve(toCopy.capacity);
	memcpy(pData, toCopy.pData, length + 1);
	return *this;
}

void String::AppendChars(const char* str, uint32_t len) {
	Reserve(GrowCapacity(length + len + 1));
	memcpy(pData + length, str, len + 1);
	length += len;
	pData[length] = '\0';
}

void String::Append(const char* str) {
	uint32_t addedLength = (uint32_t)strlen(str);
	AppendChars(str, addedLength);
}

void String::AppendFormatInternal(const char* format, va_list args) {
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

void String::AppendFormat(const char* format, ...) {
	va_list args;
	va_start(args, format);
	AppendFormatInternal(format, args);
	va_end(args);
}

void String::Clear() {
	SYS_FREE(pData);
	pData = nullptr;
	length = 0;
	capacity = 0;
}

bool String::operator==(const char* other) {
	if (strcmp(pData, other) == 0)
 		return true;
	return false;
}

bool String::operator!=(const char* other) {
	return !operator==(other);
}

bool String::operator==(const String& other) {
	return operator==(other.pData);
}

bool String::operator!=(const String& other) {
	return !operator==(other.pData);
}

char* String::begin() {
	return pData;
}

char* String::end() {
	return pData + length;
}

const char* String::begin() const{
	return pData;
}

const char* String::end() const {
	return pData + length;
}

void String::Reserve(uint32_t desiredCapacity) {
	if (capacity >= desiredCapacity) return;
	pData = (char*)SYS_REALLOC(pData, desiredCapacity * sizeof(char));
	capacity = desiredCapacity;
}

uint32_t String::GrowCapacity(uint32_t atLeastSize) const {
	// if we're big enough already, don't grow, otherwise double, 
	// and if that's not enough just use atLeastSize
	if (capacity > atLeastSize) return capacity;
	uint32_t newCapacity = capacity ? capacity * 2 : 8;
	return newCapacity > atLeastSize ? newCapacity : atLeastSize;
}