// Copyright 2020-2022 David Colson. All rights reserved.

#include "stack.h"

#include "log.h"

// ***********************************************************************

template<typename T>
Stack<T>::Stack(IAllocator* pAlloc)
    : m_array(pAlloc) {}

// ***********************************************************************

template<typename T>
void Stack<T>::Free() {
    m_array.Free();
}

// ***********************************************************************

template<typename T>
template<typename F>
void Stack<T>::Free(F&& freeElement) {
    m_array.Free(freeElement);
}

// ***********************************************************************

template<typename T>
void Stack<T>::Resize(uint32_t desiredCount) {
    m_array.Resize(desiredCount);
}

// ***********************************************************************

template<typename T>
void Stack<T>::Reserve(uint32_t desiredCapacity) {
    m_array.Reserve(desiredCapacity);
}

// ***********************************************************************

template<typename T>
void Stack<T>::Push(const T& value) {
    m_array.PushBack(value);
}

// ***********************************************************************

template<typename T>
T Stack<T>::Pop() {
    T top = m_array[m_array.m_count - 1];
    m_array.PopBack();
    return top;
}

// ***********************************************************************

template<typename T>
T& Stack<T>::Top() {
    return m_array[m_array.m_count - 1];
}

// ***********************************************************************

template<typename T>
T& Stack<T>::operator[](int32_t i) {
    if (i >= 0) {
        return m_array[i];
    } else {
        Assert(i >= -int32_t(m_array.m_count));
        return m_array[m_array.m_count + i];
    }
}

// ***********************************************************************

template<typename T>
const T& Stack<T>::operator[](int32_t i) const {
    if (i >= 0) {
        return m_array[i];
    } else {
        Assert(i >= -int32_t(m_array.m_count));
        return m_array[m_array.m_count + i];
    }
}