#include "resizable_array.h"

#include "log.h"

#include <string.h>

template<typename Type>
inline ResizableArray<Type>::ResizableArray(IAllocator* _pAlloc) {
    m_pAlloc = _pAlloc;
}

template<typename Type>
void ResizableArray<Type>::Free() {
    if (m_pData) {
        m_pAlloc->Free(m_pData);
        m_count = 0;
        m_capacity = 0;
        m_pData = nullptr;
    }
}

template<typename Type>
template<typename F>
void ResizableArray<Type>::Free(F&& freeElement) {
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

template<typename Type>
void ResizableArray<Type>::Resize(uint32_t desiredCount) {
    if (desiredCount > m_capacity) {
        Reserve(desiredCount);
    }
    m_count = desiredCount;
}

template<typename Type>
void ResizableArray<Type>::Reserve(uint32_t desiredCapacity) {
    if (m_capacity >= desiredCapacity)
        return;
    m_pData = (Type*)m_pAlloc->Reallocate(m_pData, desiredCapacity * sizeof(Type), m_capacity * sizeof(Type));
    m_capacity = desiredCapacity;
}

template<typename Type>
void ResizableArray<Type>::PushBack(const Type& value) {
    if (m_count == m_capacity) {
        Reserve(GrowCapacity(m_count + 1));
    }
    memcpy(&m_pData[m_count], &value, sizeof(Type));
    m_count++;
}

template<typename Type>
void ResizableArray<Type>::PopBack() {
    Assert(m_count > 0);
    m_count--;
}

template<typename Type>
Type& ResizableArray<Type>::operator[](size_t i) {
    Assert(i >= 0 && i < m_count);
    return m_pData[i];
}

template<typename Type>
const Type& ResizableArray<Type>::operator[](size_t i) const {
    Assert(i >= 0 && i < m_count);
    return m_pData[i];
}

template<typename Type>
void ResizableArray<Type>::Erase(size_t index) {
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

template<typename Type>
void ResizableArray<Type>::EraseUnsorted(size_t index) {
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

template<typename Type>
void ResizableArray<Type>::Insert(size_t index, const Type& value) {
    Assert(index >= 0 && index < m_count);
    if (m_capacity == m_count)
        Reserve(GrowCapacity(m_count + 1));
    memmove(m_pData + (index + 1), m_pData + index, (m_count - index) * sizeof(Type));
    memcpy(m_pData + index, &value, sizeof(Type));
    m_count++;
}

template<typename Type>
Type* ResizableArray<Type>::Find(const Type& value) {
    Type* pTest = m_pData;
    const Type* pDataEnd = m_pData + m_count;
    while (pTest < pDataEnd) {
        if (*pTest == value)
            break;
        pTest++;
    }
    return pTest;
}

template<typename Type>
Type* ResizableArray<Type>::begin() {
    return m_pData;
}

template<typename Type>
Type* ResizableArray<Type>::end() {
    return m_pData + m_count;
}

template<typename Type>
const Type* ResizableArray<Type>::begin() const {
    return m_pData;
}

template<typename Type>
const Type* ResizableArray<Type>::end() const {
    return m_pData + m_count;
}

template<typename Type>
uint32_t ResizableArray<Type>::IndexFromPointer(const Type* ptr) const {
    Assert(ptr >= m_pData && ptr < m_pData + m_count);
    ptrdiff_t diff = ptr - m_pData;
    return (uint32_t)diff;
}

template<typename Type>
bool ResizableArray<Type>::Validate() const {
    return m_capacity >= m_count;
}

template<typename Type>
uint32_t ResizableArray<Type>::GrowCapacity(uint32_t atLeastSize) const {
    // if we're big enough already, don't grow, otherwise double,
    // and if that's not enough just use atLeastSize
    if (m_capacity > atLeastSize)
        return m_capacity;
    uint32_t newCapacity = m_capacity ? m_capacity * 2 : 8;
    return newCapacity > atLeastSize ? newCapacity : atLeastSize;
}