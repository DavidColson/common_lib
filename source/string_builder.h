// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

#include "light_string.h"

// String Builder
// ---------------------
// A dynamic char buffer used to construct strings
// Allocates memory on resize, must be freed manually
// TODO: Documentation


struct StringBuilder {
    char* m_pData = nullptr;
    size_t m_length = 0;
    size_t m_capacity = 0;
    IAllocator* m_pAlloc { nullptr };

    StringBuilder(IAllocator* _pAlloc = &g_Allocator) {
        m_pAlloc = _pAlloc;
    }

    void AppendChars(const char* str, size_t len) {
        Reserve(GrowCapacity(m_length + len + 1));
        memcpy(m_pData + m_length, str, len);
        m_length += len;
        m_pData[m_length] = 0;
    }

    void Append(const char* str) {
        size_t addedLength = strlen(str);
        AppendChars(str, addedLength);
    }

    void Append(String str) {
        AppendChars(str.m_pData, str.m_length);
    }

    void AppendFormatInternal(const char* format, va_list args) {
        va_list argsCopy;
        va_copy(argsCopy, args);

        int addedLength = vsnprintf(nullptr, 0, format, args);
        if (addedLength <= 0) {
            va_end(argsCopy);
            return;
        }

        Reserve(GrowCapacity(m_length + addedLength));
        vsnprintf(m_pData + m_length, addedLength + 1, format, args);
        va_end(argsCopy);
        m_length += addedLength;
    }

    void AppendFormat(const char* format, ...) {
        va_list args;
        va_start(args, format);
        AppendFormatInternal(format, args);
        va_end(args);
    }

    String CreateString(bool reset = true, IAllocator* _pAlloc = &g_Allocator) {
        String output = AllocString(m_length + 1, _pAlloc);
        memcpy(output.m_pData, m_pData, m_length * sizeof(char));
        output.m_pData[m_length] = 0;
        output.m_length = m_length;

        if (reset)
            Reset();
        return output;
    }

    void Reset() {
        if (m_pData) {
            m_pAlloc->Free(m_pData);
            m_pData = nullptr;
            m_length = 0;
            m_capacity = 0;
        }
    }

    void Reserve(size_t desiredCapacity) {
        if (m_capacity >= desiredCapacity)
            return;
        m_pData = (char*)m_pAlloc->Reallocate(m_pData, desiredCapacity * sizeof(char), m_capacity * sizeof(char));
        m_capacity = desiredCapacity;
    }

    size_t GrowCapacity(size_t atLeastSize) const {
        // if we're big enough already, don't grow, otherwise double,
        // and if that's not enough just use atLeastSize
        if (m_capacity > atLeastSize)
            return m_capacity;
        size_t newCapacity = m_capacity ? m_capacity * 2 : 8;
        return newCapacity > atLeastSize ? newCapacity : atLeastSize;
    }
};
