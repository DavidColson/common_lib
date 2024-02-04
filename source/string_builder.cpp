#include "string_builder.h"

#include "light_string.h"

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

StringBuilder::StringBuilder(IAllocator* _pAlloc) {
    pAlloc = _pAlloc;
}

void StringBuilder::AppendChars(const char* str, usize len) {
    Reserve(GrowCapacity(length + len + 1));
    memcpy(pData + length, str, len);
    length += len;
    pData[length] = 0;
}

void StringBuilder::Append(const char* str) {
    usize addedLength = strlen(str);
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

String StringBuilder::CreateString(bool reset, IAllocator* _pAlloc) {
    String output = AllocString(length + 1, _pAlloc);
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
        pAlloc->Free(pData);
        pData = nullptr;
        length = 0;
        capacity = 0;
    }
}

void StringBuilder::Reserve(usize desiredCapacity) {
    if (capacity >= desiredCapacity)
        return;
    pData = (char*)pAlloc->Reallocate(pData, desiredCapacity * sizeof(byte), capacity * sizeof(byte));
    capacity = desiredCapacity;
}

usize StringBuilder::GrowCapacity(usize atLeastSize) const {
    // if we're big enough already, don't grow, otherwise f64,
    // and if that's not enough just use atLeastSize
    if (capacity > atLeastSize)
        return capacity;
    usize newCapacity = capacity ? capacity * 2 : 8;
    return newCapacity > atLeastSize ? newCapacity : atLeastSize;
}
