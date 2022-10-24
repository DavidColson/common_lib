
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


#ifdef _DEBUG
#define DEBUG_CHECK(expression) if (!(expression)) __debugbreak();
#else
#define DEBUG_CHECK(expression)
#endif

// Dynamic Array Structure
// -----------------------
// ... todo documentation, examples, reasoning etc

template<typename type>
struct Array {
    type* pData{ nullptr };
    uint32_t count{ 0 };
	uint32_t capacity { 0 };
	
	~Array() {
		if (pData) free(pData);
	}

	// TODO: Need a resize function, that doesn't free memory, so it can be reused

	void reserve(uint32_t desiredCapacity) {
		if (capacity >= desiredCapacity) return;
		pData = (type*)realloc(pData, desiredCapacity * sizeof(type));
		capacity = desiredCapacity;
	}

	void push_back(const type& value) {
		if (count == capacity) {
			reserve(grow_capacity(count + 1));
		}
		memcpy(&pData[count], &value, sizeof(type));
		count++;
	}

	void pop_back() {
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

	void clear() {
		free(pData);
		pData = nullptr;
		count = 0;
		capacity = 0;
	}

	void erase(uint32_t index) {
		DEBUG_CHECK(index >= 0 && index < count);
		if (index == count-1) {
			pop_back(); count--;
		}
		if (index < count-1) {
			memmove(pData + index, pData + (index + 1), (count - index) * sizeof(type));
			count--;
		}
	}

	void erase_unsorted(uint32_t index) {
		DEBUG_CHECK(index >= 0 && index < count);
		if (index == count-1) {
			pop_back(); count--;
		}
		if (index < count-1) {
			memcpy(pData + index, pData + (count - 1), sizeof(type));
			count--;
		}
	}
	
	void insert(uint32_t index, const type& value) {
		DEBUG_CHECK(index >= 0 && index < count);
		if (capacity == count) reserve(grow_capacity(count + 1));
		memmove(pData + (index + 1), pData + index, (count-index) * sizeof(type));
		memcpy(pData + index, &value, sizeof(type));
		count++;
	}

	type* find(const type& value) {
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

	uint32_t index_from_ptr(const type* ptr) const {
		DEBUG_CHECK(ptr >= pData && ptr < pData + count);
		ptrdiff_t diff = ptr - pData;
		return (uint32_t)diff;
	}

	bool validate() const {
		return capacity >= count;
	}

	uint32_t grow_capacity(uint32_t atLeastSize) const {
		// if we're big enough already, don't grow, otherwise double, 
		// and if that's not enough just use atLeastSize
		if (capacity > atLeastSize) return capacity;
		uint32_t newCapacity = capacity ? capacity * 2 : 8;
		return newCapacity > atLeastSize ? newCapacity : atLeastSize;
	}
};