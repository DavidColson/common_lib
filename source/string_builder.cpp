
// ***********************************************************************

StringBuilder::StringBuilder(Arena* _pArena) {
    pArena = _pArena;
}

// ***********************************************************************

void StringBuilder::AppendChars(const char* str, u64 len) {
    Reserve(GrowCapacity(length + len + 1));
    memcpy(pData + length, str, len);
    length += len;
    pData[length] = 0;
}

// ***********************************************************************

void StringBuilder::Append(const char* str) {
    u64 addedLength = strlen(str);
    AppendChars(str, addedLength);
}

// ***********************************************************************

void StringBuilder::Append(String str) {
    AppendChars(str.pData, str.length);
}

// ***********************************************************************

bool IsFormatSpecifier(char c) {
	switch (c) {
		case 'd': return true;
		case 'i': return true;
		case 'u': return true;
		case 'o': return true;
		case 'x': return true;
		case 'X': return true;
		case 'f': return true;
		case 'F': return true;
		case 'e': return true;
		case 'E': return true;
		case 'g': return true;
		case 'G': return true;
		case 'a': return true;
		case 'A': return true;
		case 'c': return true;
		case 's': return true; 
		case 'S': return true; 
		case 'p': return true; 
		case 'n': return true; 
		case '%': return true;
		default: return false;
	}
}

// ***********************************************************************

void StringBuilder::AppendFormatInternal(const char* format, va_list args) {
	char formatHelper[256];
	memset(formatHelper, 0, 256);

    while(*format!='\0')
    {
		// non formatting character, just add it
        if(*format!='%')
        {
			AppendChars(format, 1);
            format++;
            continue;
        }

		const char* formatStart = format;
		bool requiresCount = false;
		format++;
		while (!IsFormatSpecifier(*format)) {
			if (*(format++) == '*') {
				requiresCount = true;
			}
		}
		if (*format == 'S') {
			// special handling for our non-null terminated string
			String str = va_arg(args, String);
			i32 addedLength = snprintf(nullptr, 0, "%.*s", (i32)str.length, str.pData);
			Reserve(GrowCapacity(length + addedLength + 1));
			snprintf(pData + length, addedLength + 1, "%.*s", (i32)str.length, str.pData);
			length += addedLength;
			format++;
		}
		else {
			// default handling for everything else
			format++;
			memcpy(formatHelper, formatStart, format-formatStart);

			// formatHelper now contains the format we want to process
			i32 addedLength = vsnprintf(nullptr, 0, formatHelper, args);
			Reserve(GrowCapacity(length + addedLength + 1));
			vsnprintf(pData + length, addedLength + 1, formatHelper, args);
			length += addedLength;

			va_arg(args, int); // skip element since the above copied the arg list
			if (requiresCount)
				va_arg(args, int);

			// reset format for next time
			memset(formatHelper, 0, 256);
		}
    }
}

// ***********************************************************************

void StringBuilder::AppendFormat(const char* format, ...) {
    va_list args;
    va_start(args, format);
    AppendFormatInternal(format, args);
    va_end(args);
}

// ***********************************************************************

String StringBuilder::CreateString(Arena* pArena, bool reset) {
    String output = AllocString(length + 1, pArena);
    memcpy(output.pData, pData, length * sizeof(char));
    output.pData[length] = 0;
    output.length = length;

    if (reset)
        Reset();
    return output;
}

// ***********************************************************************

String StringBuilder::ToExistingString(bool reset, String& destination) {
    memcpy(destination.pData, pData, length * sizeof(char));
    destination.pData[length] = 0;
    destination.length = length;

    if (reset)
        Reset();
    return destination;
}

// ***********************************************************************

void StringBuilder::Reset() {
    if (pData) {
		pData = nullptr;
        length = 0;
        capacity = 0;
    }
}

// ***********************************************************************

void StringBuilder::Reserve(u64 desiredCapacity) {
    if (capacity >= desiredCapacity)
        return;
    pData = (char*)ArenaRealloc(pArena, pData, desiredCapacity * sizeof(char), capacity * sizeof(char), alignof(char));
    capacity = desiredCapacity;
}

// ***********************************************************************

u64 StringBuilder::GrowCapacity(u64 atLeastSize) const {
    // if we're big enough already, don't grow, otherwise f64,
    // and if that's not enough just use atLeastSize
    if (capacity > atLeastSize)
        return capacity;
    u64 newCapacity = capacity ? capacity * 2 : 8;
    return newCapacity > atLeastSize ? newCapacity : atLeastSize;
}

// ***********************************************************************

String StringPrint(Arena* pArena, const char* format, ...) {
    va_list args;
    va_start(args, format);
	
	StringBuilder builder(g_pArenaFrame);
	builder.AppendFormatInternal(format, args);
	String result = builder.CreateString(pArena);

    va_end(args);
	return result;
}

// ***********************************************************************

String TempPrint(const char* format, ...) {
    va_list args;
    va_start(args, format);
	
	StringBuilder builder(g_pArenaFrame);
	builder.AppendFormatInternal(format, args);
	String result = builder.CreateString(g_pArenaFrame);

    va_end(args);
	return result;
}
