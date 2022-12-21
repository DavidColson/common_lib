#pragma once

#include "light_string.h"

// String Builder
// ---------------------
// A dynamic char buffer used to construct strings
// Allocates memory on resize, must be freed manually
// TODO: Documentation


struct StringBuilder {
    char* pData = nullptr;
    size_t length = 0;
    size_t capacity = 0;
    IAllocator* pAlloc { nullptr };

    StringBuilder(IAllocator* _pAlloc = &gAllocator) {
        pAlloc = _pAlloc;
    }

    void AppendChars(const char* str, size_t len) {
        Reserve(GrowCapacity(length + len + 1));
        memcpy(pData + length, str, len);
        length += len;
        pData[length] = 0;
    }

    void Append(const char* str) {
        size_t addedLength = strlen(str);
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

    String CreateString(bool reset = true, IAllocator* _pAlloc = &gAllocator) {
        String output = AllocString(length + 1, _pAlloc);
        memcpy(output.pData, pData, length * sizeof(char));
        output.pData[length] = 0;
        output.length = length;

        if (reset)
            Reset();
        return output;
    }

    void Reset() {
        if (pData) {
            pAlloc->Free(pData);
            pData = nullptr;
            length = 0;
            capacity = 0;
        }
    }

    void Reserve(size_t desiredCapacity) {
        if (capacity >= desiredCapacity)
            return;
        pData = (char*)pAlloc->Reallocate(pData, desiredCapacity * sizeof(char), capacity * sizeof(char));
        capacity = desiredCapacity;
    }

    size_t GrowCapacity(size_t atLeastSize) const {
        // if we're big enough already, don't grow, otherwise double,
        // and if that's not enough just use atLeastSize
        if (capacity > atLeastSize)
            return capacity;
        size_t newCapacity = capacity ? capacity * 2 : 8;
        return newCapacity > atLeastSize ? newCapacity : atLeastSize;
    }
};
