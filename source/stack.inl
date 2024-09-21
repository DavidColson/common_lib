// Copyright 2020-2022 David Colson. All rights reserved.

#include "stack.h"

#include "log.h"

// ***********************************************************************

template<typename T>
Stack<T>::Stack(Arena* pArena)
    : array(pArena) {}

// ***********************************************************************

template<typename T>
void Stack<T>::Resize(size desiredCount) {
    array.Resize(desiredCount);
}

// ***********************************************************************

template<typename T>
void Stack<T>::Reserve(size desiredCapacity) {
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
T& Stack<T>::operator[](size i) {
    if (i >= 0) {
        return array[i];
    } else {
        Assert(i >= -size(array.count));
        return array[array.count + i];
    }
}

// ***********************************************************************

template<typename T>
const T& Stack<T>::operator[](size i) const {
    if (i >= 0) {
        return array[i];
    } else {
        Assert(i >= -size(array.count));
        return array[array.count + i];
    }
}
