#pragma once

#include "memory.h"
#include "log.h"

#include <stdint.h>
#include <string.h>

// Resizable Array Structure
// -----------------------
// Must be freed manually
// ... todo documentation, examples, reasoning etc

template<typename type>
struct ResizableArray {
    type* pData{ nullptr };
    uint32_t count{ 0 };
	uint32_t capacity { 0 };
	IAllocator* pAlloc { nullptr };
	
	ResizableArray(IAllocator* _pAlloc = &gAllocator) {
		pAlloc = _pAlloc;
	}

	void Free() {
		if (pData) {
			pAlloc->Free(pData);
			count = 0;
			capacity = 0;
			pData = nullptr;
		}
	}

	template<typename F>
	void Free(F&& freeElement) {
		if (pData) {
			for (uint32_t i = 0; i < count; i++) {
				freeElement(pData[i]);
			}
			pAlloc->Free(pData);
			count = 0;
			capacity = 0;
			pData = nullptr;
		}
	}

	// TODO: Need a resize function, that doesn't free memory, so it can be reused

	void Reserve(uint32_t desiredCapacity) {
		if (capacity >= desiredCapacity) return;
		pData = (type*)pAlloc->Reallocate(pData, desiredCapacity * sizeof(type), capacity * sizeof(type));
		capacity = desiredCapacity;
	}

	void PushBack(const type& value) {
		if (count == capacity) {
			Reserve(GrowCapacity(count + 1));
		}
		memcpy(&pData[count], &value, sizeof(type));
		count++;
	}

	void PopBack() {
		Assert(count > 0);
		count--;
	}

	type& operator[](size_t i) {
		Assert(i >= 0 && i < count);
		return pData[i];
	}
	
	const type& operator[](size_t i) const {
		Assert(i >= 0 && i < count);
		return pData[i];
	}

	void Reset() {
		count = 0;
		capacity = 0;
	}

	void Erase(size_t index) {
		Assert(index >= 0 && index < count);
		if (index == count-1) {
			PopBack(); count--;
		}
		if (index < count-1) {
			memmove(pData + index, pData + (index + 1), (count - index) * sizeof(type));
			count--;
		}
	}

	void EraseUnsorted(size_t index) {
		Assert(index >= 0 && index < count);
		if (index == count-1) {
			PopBack(); count--;
		}
		if (index < count-1) {
			memcpy(pData + index, pData + (count - 1), sizeof(type));
			count--;
		}
	}
	
	void Insert(size_t index, const type& value) {
		Assert(index >= 0 && index < count);
		if (capacity == count) Reserve(GrowCapacity(count + 1));
		memmove(pData + (index + 1), pData + index, (count-index) * sizeof(type));
		memcpy(pData + index, &value, sizeof(type));
		count++;
	}

	type* Find(const type& value) {
		type* pTest = pData;
		const type* pDataEnd = pData + count;
		while (pTest < pDataEnd) {
			if (*pTest == value)
				break;
			pTest++;
		}
		return pTest;
	}

	type* begin() {
		return pData;
	}

	type* end() {
		return pData + count;
	}

	const type* begin() const{
		return pData;
	}

	const type* end() const {
		return pData + count;
	}

	uint32_t IndexFromPointer(const type* ptr) const {
		Assert(ptr >= pData && ptr < pData + count);
		ptrdiff_t diff = ptr - pData;
		return (uint32_t)diff;
	}

	bool Validate() const {
		return capacity >= count;
	}

	uint32_t GrowCapacity(uint32_t atLeastSize) const {
		// if we're big enough already, don't grow, otherwise double, 
		// and if that's not enough just use atLeastSize
		if (capacity > atLeastSize) return capacity;
		uint32_t newCapacity = capacity ? capacity * 2 : 8;
		return newCapacity > atLeastSize ? newCapacity : atLeastSize;
	}
};