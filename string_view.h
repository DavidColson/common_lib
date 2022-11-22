#pragma once

#include "string.h"

#include <stdint.h>
#include <string.h>

// String view
// -------------------------------
// ... Documentation
// non-null terminated string view
// does not own data, guaranteed to do no mem ops, very fast

struct StringView {
	const char *pData = nullptr;
	uint32_t length = 0;

	StringView(const char* str) {
		pData = str;
		length = (uint32_t)strlen(str);
	}

	StringView(const String& str) {
		pData = str.pData;
		length = str.length;
	}

	void operator=(const char* str) {
		pData = str;
		length = (uint32_t)strlen(str);
	}
	
	void operator=(const String& str) {
		pData = str.pData;
		length = str.length;
	}
};