// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

// Stack Structure
// -----------------------
// Must be freed manually
// Can use negative indices (i.e. -1 is top of stack, -count is bottom of stack)
// ... todo documentation, examples, reasoning etc

template<typename T>
struct Stack {
    ResizableArray<T> array;

    Stack(Arena* pArena);

    void Resize(i64 desiredCount);

    void Reserve(i64 desiredCapacity);

    void Push(const T& value);

    T Pop();

    T& Top();

    T& operator[](i64 i);

    const T& operator[](i64 i) const;
};



// IMPLEMENTATION


// ***********************************************************************

template<typename T>
Stack<T>::Stack(Arena* pArena)
    : array(pArena) {}

// ***********************************************************************

template<typename T>
void Stack<T>::Resize(i64 desiredCount) {
    array.Resize(desiredCount);
}

// ***********************************************************************

template<typename T>
void Stack<T>::Reserve(i64 desiredCapacity) {
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
T& Stack<T>::operator[](i64 i) {
    if (i >= 0) {
        return array[i];
    } else {
        Assert(i >= -i64(array.count));
        return array[array.count + i];
    }
}

// ***********************************************************************

template<typename T>
const T& Stack<T>::operator[](i64 i) const {
    if (i >= 0) {
        return array[i];
    } else {
        Assert(i >= -i64(array.count));
        return array[array.count + i];
    }
}
