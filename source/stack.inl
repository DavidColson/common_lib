// Copyright 2020-2022 David Colson. All rights reserved.

#include "stack.h"

#include "log.h"

// ***********************************************************************

template<typename T>
Stack<T>::Stack(IAllocator* pAlloc)
    : array(pAlloc) {}

// ***********************************************************************

template<typename T>
void Stack<T>::Free() {
    array.Free();
}

// ***********************************************************************

template<typename T>
template<typename F>
void Stack<T>::Free(F&& freeElement) {
    array.Free(freeElement);
}

// ***********************************************************************

template<typename T>
void Stack<T>::Resize(uint32_t desiredCount) {
    array.Resize(desiredCount);
}

// ***********************************************************************

template<typename T>
void Stack<T>::Reserve(uint32_t desiredCapacity) {
    array.Reserve(desiredCapacity);
}

// ***********************************************************************

template<typename T>
void Stack<T>::Push(const T& value) {
    array.PushBack(value);
}

// ***********************************************************************

template<typename T>
T Stack<T>::Pop() {
    T top = array[array.count - 1];
    array.PopBack();
    return top;
}

// ***********************************************************************

template<typename T>
T& Stack<T>::Top() {
    return array[array.count - 1];
}

// ***********************************************************************

template<typename T>
T& Stack<T>::operator[](int32_t i) {
    if (i >= 0) {
        return array[i];
    } else {
        Assert(i >= -int32_t(array.count));
        return array[array.count + i];
    }
}

// ***********************************************************************

template<typename T>
const T& Stack<T>::operator[](int32_t i) const {
    if (i >= 0) {
        return array[i];
    } else {
        Assert(i >= -int32_t(array.count));
        return array[array.count + i];
    }
}