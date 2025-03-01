
StringBuilder::StringBuilder(Arena* _pArena) {
    pArena = _pArena;
}

void StringBuilder::AppendChars(const char* str, u64 len) {
    Reserve(GrowCapacity(length + len + 1));
    memcpy(pData + length, str, len);
    length += len;
    pData[length] = 0;
}

void StringBuilder::Append(const char* str) {
    u64 addedLength = strlen(str);
    AppendChars(str, addedLength);
}

void StringBuilder::Append(String str) {
    AppendChars(str.pData, str.length);
}

void StringBuilder::AppendFormatInternal(const char* format, va_list args) {
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

void StringBuilder::AppendFormat(const char* format, ...) {
    va_list args;
    va_start(args, format);
    AppendFormatInternal(format, args);
    va_end(args);
}

String StringBuilder::CreateString(Arena* pArena, bool reset) {
    String output = AllocString(length + 1, pArena);
    memcpy(output.pData, pData, length * sizeof(char));
    output.pData[length] = 0;
    output.length = length;

    if (reset)
        Reset();
    return output;
}

String StringBuilder::ToExistingString(bool reset, String& destination) {
    memcpy(destination.pData, pData, length * sizeof(char));
    destination.pData[length] = 0;
    destination.length = length;

    if (reset)
        Reset();
    return destination;
}

void StringBuilder::Reset() {
    if (pData) {
		pData = nullptr;
        length = 0;
        capacity = 0;
    }
}

void StringBuilder::Reserve(u64 desiredCapacity) {
    if (capacity >= desiredCapacity)
        return;
    pData = (char*)ArenaRealloc(pArena, pData, desiredCapacity * sizeof(char), capacity * sizeof(char), alignof(char));
    capacity = desiredCapacity;
}

u64 StringBuilder::GrowCapacity(u64 atLeastSize) const {
    // if we're big enough already, don't grow, otherwise f64,
    // and if that's not enough just use atLeastSize
    if (capacity > atLeastSize)
        return capacity;
    u64 newCapacity = capacity ? capacity * 2 : 8;
    return newCapacity > atLeastSize ? newCapacity : atLeastSize;
}
