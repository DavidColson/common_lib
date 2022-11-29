#include "string.h"

String CopyCString(IAllocator* pAlloc, const char* string) {
	String s;
	size_t len = strlen(string);
	s.pData = (char*)pAlloc->Allocate((len+1) * sizeof(char));
	s.length = len;
	memcpy(s.pData, string, (len+1) * sizeof(char));
	return s;
}

String CopyCStringRange(IAllocator* pAlloc, char* start, char* end) {
	String s;
	size_t len = end - start;
	s.pData = (char*)pAlloc->Allocate((len+1) * sizeof(char));
	s.length = len;
	memcpy(s.pData, start, len * sizeof(char));
	s.pData[s.length] = 0;
	return s;
}

String CopyString(IAllocator* pAlloc, String& string) {
	String s;
	s.pData = (char*)pAlloc->Allocate((string.length+1) * sizeof(char));
	s.length = string.length;
	memcpy(s.pData, string.pData, string.length * sizeof(char));
	s.pData[string.length] = 0;
	return s;
}

String AllocString(IAllocator* pAlloc, size_t length) {
	String s;
	s.pData = (char*)pAlloc->Allocate(length * sizeof(char));
	s.length = length;
	return s;
}

void FreeString(IAllocator* pAlloc, String& string) {
	pAlloc->Free(string.pData);
	string.pData = nullptr;
	string.length = 0;
}