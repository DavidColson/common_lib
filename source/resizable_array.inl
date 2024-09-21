#include "resizable_array.h"

#include "log.h"

#include <string.h>

template<typename Type>
inline ResizableArray<Type>::ResizableArray(Arena* _pArena) {
	pArena = _pArena;
}

template<typename Type>
void ResizableArray<Type>::Resize(size desiredCount) {
    if (desiredCount > capacity) {
        Reserve(desiredCount);
    }
    count = desiredCount;
}

template<typename Type>
void ResizableArray<Type>::Reserve(size desiredCapacity) {
    if (capacity >= desiredCapacity)
        return;
	pData = (Type*)ArenaRealloc(pArena, pData, desiredCapacity * sizeof(Type), capacity * sizeof(Type), alignof(Type));
    capacity = desiredCapacity;
}

template<typename Type>
void ResizableArray<Type>::PushBack(const Type& value) {
    if (count == capacity) {
        Reserve(GrowCapacity(count + 1));
    }
    memcpy(&pData[count], &value, sizeof(Type));
    count++;
}

template<typename Type>
void ResizableArray<Type>::PopBack() {
    Assert(count > 0);
    count--;
}

template<typename Type>
Type& ResizableArray<Type>::operator[](size i) {
    Assert(i >= 0 && i < count);
    return pData[i];
}

template<typename Type>
const Type& ResizableArray<Type>::operator[](size i) const {
    Assert(i >= 0 && i < count);
    return pData[i];
}

template<typename Type>
void ResizableArray<Type>::Erase(size index) {
    Assert(index >= 0 && index < count);
    if (index == count - 1) {
        PopBack();
        return;
    }
    if (index < count - 1) {
        memmove(pData + index, pData + (index + 1), (count - index - 1) * sizeof(Type));
        count--;
    }
}

template<typename Type>
void ResizableArray<Type>::EraseUnsorted(size index) {
    Assert(index >= 0 && index < count);
    if (index == count - 1) {
        PopBack();
        count--;
    }
    if (index < count - 1) {
        memcpy(pData + index, pData + (count - 1), sizeof(Type));
        count--;
    }
}

template<typename Type>
void ResizableArray<Type>::Insert(size index, const Type& value) {
    Assert(index >= 0 && index < count);
    if (capacity == count)
        Reserve(GrowCapacity(count + 1));
    memmove(pData + (index + 1), pData + index, (count - index) * sizeof(Type));
    memcpy(pData + index, &value, sizeof(Type));
    count++;
}

template<typename Type>
Type* ResizableArray<Type>::Find(const Type& value) {
    Type* pTest = pData;
    const Type* pDataEnd = pData + count;
    while (pTest < pDataEnd) {
        if (*pTest == value)
            break;
        pTest++;
    }
    return pTest;
}

template<typename Type>
Type* ResizableArray<Type>::begin() {
    return pData;
}

template<typename Type>
Type* ResizableArray<Type>::end() {
    return pData + count;
}

template<typename Type>
const Type* ResizableArray<Type>::begin() const {
    return pData;
}

template<typename Type>
const Type* ResizableArray<Type>::end() const {
    return pData + count;
}

template<typename Type>
size ResizableArray<Type>::IndexFromPointer(const Type* ptr) const {
    Assert(ptr >= pData && ptr < pData + count);
    size diff = ptr - pData;
    return diff;
}

template<typename Type>
bool ResizableArray<Type>::Validate() const {
    return capacity >= count;
}

template<typename Type>
size ResizableArray<Type>::GrowCapacity(size atLeastSize) const {
    // if we're big enough already, don't grow, otherwise f64,
    // and if that's not enough just use atLeastSize
    if (capacity > atLeastSize)
        return capacity;
    size newCapacity = capacity ? capacity * 2 : 8;
    return newCapacity > atLeastSize ? newCapacity : atLeastSize;
}
