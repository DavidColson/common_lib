#include "string_view.h"

#include "string.h"

StringView::StringView(const char* str) {
	pData = str;
	length = (uint32_t)strlen(str);
}

StringView::StringView(const String& str) {
	pData = str.pData;
	length = str.length;
}

void StringView::operator=(const char* str) {
	pData = str;
	length = (uint32_t)strlen(str);
}

void StringView::operator=(const String& str) {
	pData = str.pData;
	length = str.length;
}