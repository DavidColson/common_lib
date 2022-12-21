// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

#include "log.h"
#include "memory.h"

#include <stdint.h>
#include <string.h>

// Resizable Array Structure
// -----------------------
// Must be freed manually
// ... todo documentation, examples, reasoning etc

template<typename Type>
struct ResizableArray {
    Type* m_pData { nullptr };
    uint32_t m_count { 0 };
    uint32_t m_capacity { 0 };
    IAllocator* m_pAlloc { nullptr };

    ResizableArray(IAllocator* _pAlloc = &g_Allocator) {
        m_pAlloc = _pAlloc;
    }

    void Free() {
        if (m_pData) {
            m_pAlloc->Free(m_pData);
            m_count = 0;
            m_capacity = 0;
            m_pData = nullptr;
        }
    }

    template<typename F>
    void Free(F&& freeElement) {
        if (m_pData) {
            for (uint32_t i = 0; i < m_count; i++) {
                freeElement(m_pData[i]);
            }
            m_pAlloc->Free(m_pData);
            m_count = 0;
            m_capacity = 0;
            m_pData = nullptr;
        }
    }

    void Resize(uint32_t desiredCount) {
        if (desiredCount > m_capacity) {
            Reserve(desiredCount);
        }
        m_count = desiredCount;
    }

    void Reserve(uint32_t desiredCapacity) {
        if (m_capacity >= desiredCapacity)
            return;
        m_pData = (Type*)m_pAlloc->Reallocate(m_pData, desiredCapacity * sizeof(Type), m_capacity * sizeof(Type));
        m_capacity = desiredCapacity;
    }

    void PushBack(const Type& value) {
        if (m_count == m_capacity) {
            Reserve(GrowCapacity(m_count + 1));
        }
        memcpy(&m_pData[m_count], &value, sizeof(Type));
        m_count++;
    }

    void PopBack() {
        Assert(m_count > 0);
        m_count--;
    }

    Type& operator[](size_t i) {
        Assert(i >= 0 && i < m_count);
        return m_pData[i];
    }

    const Type& operator[](size_t i) const {
        Assert(i >= 0 && i < m_count);
        return m_pData[i];
    }

    void Erase(size_t index) {
        Assert(index >= 0 && index < m_count);
        if (index == m_count - 1) {
            PopBack();
            m_count--;
        }
        if (index < m_count - 1) {
            memmove(m_pData + index, m_pData + (index + 1), (m_count - index - 1) * sizeof(Type));
            m_count--;
        }
    }

    void EraseUnsorted(size_t index) {
        Assert(index >= 0 && index < m_count);
        if (index == m_count - 1) {
            PopBack();
            m_count--;
        }
        if (index < m_count - 1) {
            memcpy(m_pData + index, m_pData + (m_count - 1), sizeof(Type));
            m_count--;
        }
    }

    void Insert(size_t index, const Type& value) {
        Assert(index >= 0 && index < m_count);
        if (m_capacity == m_count)
            Reserve(GrowCapacity(m_count + 1));
        memmove(m_pData + (index + 1), m_pData + index, (m_count - index) * sizeof(Type));
        memcpy(m_pData + index, &value, sizeof(Type));
        m_count++;
    }

    Type* Find(const Type& value) {
        Type* pTest = m_pData;
        const Type* pDataEnd = m_pData + m_count;
        while (pTest < pDataEnd) {
            if (*pTest == value)
                break;
            pTest++;
        }
        return pTest;
    }

    Type* begin() {
        return m_pData;
    }

    Type* end() {
        return m_pData + m_count;
    }

    const Type* begin() const {
        return m_pData;
    }

    const Type* end() const {
        return m_pData + m_count;
    }

    uint32_t IndexFromPointer(const Type* ptr) const {
        Assert(ptr >= m_pData && ptr < m_pData + m_count);
        ptrdiff_t diff = ptr - m_pData;
        return (uint32_t)diff;
    }

    bool Validate() const {
        return m_capacity >= m_count;
    }

    uint32_t GrowCapacity(uint32_t atLeastSize) const {
        // if we're big enough already, don't grow, otherwise double,
        // and if that's not enough just use atLeastSize
        if (m_capacity > atLeastSize)
            return m_capacity;
        uint32_t newCapacity = m_capacity ? m_capacity * 2 : 8;
        return newCapacity > atLeastSize ? newCapacity : atLeastSize;
    }
};
