#include <stdint.h>
#include <string.h>

// String view
// -------------------------------
// ... Documentation
// non-null terminated string view
// does not own data, guaranteed to do no mem ops, very fast

struct String;

struct StringView {
	const char *pData = nullptr;
	uint32_t length = 0;

	StringView(const char* str);

	StringView(const String& str);

	void operator=(const char* str);
	
	void operator=(const String& str);
};