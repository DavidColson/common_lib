#pragma once

#include "memory.h"

#include <stdint.h>
#include <string.h>

#ifdef _DEBUG
#define DEBUG_CHECK(expression) if (!(expression)) __debugbreak();
#else
#define DEBUG_CHECK(expression)
#endif

// Dynamic Array Structure
// -----------------------
// ... todo documentation, examples, reasoning etc

template<typename type, typename AllocatorType = Allocator>
struct Array {
    type* pData{ nullptr };
    uint32_t count{ 0 };
	uint32_t capacity { 0 };
	AllocatorType allocator;
	
	void Free() {
		if (pData) { allocator.Free(pData); }
	}

	template<typename F>
	void Free(F&& freeElement) {
		if (pData) {
			for (uint32_t i = 0; i < count; i++) {
				freeElement(pData[i]);
			}
			allocator.Free(pData);
		}
	}

	// TODO: Need a resize function, that doesn't free memory, so it can be reused

	void Reserve(uint32_t desiredCapacity) {
		if (capacity >= desiredCapacity) return;
		pData = (type*)allocator.Reallocate(pData, desiredCapacity * sizeof(type));
		capacity = desiredCapacity;
	}

	void PushBack(const type& value) {
		if (count == capacity) {
			Reserve(GrowCapacity(count + 1));
		}
		SYS_P_NEW(pData + count) type(value);
		count++;
	}

	void PopBack() {
		DEBUG_CHECK(count > 0);
		count--;
	}

	type& operator[](uint32_t i) {
		DEBUG_CHECK(i >= 0 && i < count);
		return pData[i];
	}
	
	const type& operator[](uint32_t i) const {
		DEBUG_CHECK(i >= 0 && i < count);
		return pData[i];
	}

	void Clear() {
		allocator.Free(pData);
		pData = nullptr;
		count = 0;
		capacity = 0;
	}

	void Erase(uint32_t index) {
		DEBUG_CHECK(index >= 0 && index < count);
		if (index == count-1) {
			PopBack(); count--;
		}
		if (index < count-1) {
			memmove(pData + index, pData + (index + 1), (count - index) * sizeof(type));
			count--;
		}
	}

	void EraseUnsorted(uint32_t index) {
		DEBUG_CHECK(index >= 0 && index < count);
		if (index == count-1) {
			PopBack(); count--;
		}
		if (index < count-1) {
			memcpy(pData + index, pData + (count - 1), sizeof(type));
			count--;
		}
	}
	
	void Insert(uint32_t index, const type& value) {
		DEBUG_CHECK(index >= 0 && index < count);
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
		DEBUG_CHECK(ptr >= pData && ptr < pData + count);
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