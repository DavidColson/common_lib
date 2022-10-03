
#include "stdio.h"
#include "stdlib.h"
#include "string.h"


// Core Types
typedef unsigned int uint32;

#ifdef _DEBUG
#define DEBUG_CHECK(expression) if (!(expression)) __debugbreak();
#else
#define DEBUG_CHECK(expression)
#endif

template<typename type>
struct Array {
    type* pData{ nullptr };
    uint32 count{ 0 };
	uint32 capacity { 0 };
	
	void reserve(uint32 desiredCapacity) {
		if (capacity >= desiredCapacity) return;
		type* pNewData = (type*)malloc(desiredCapacity * sizeof(type));
		if (pData) {
			memcpy(pNewData, pData, count * sizeof(type));
			free(pData);
		}
		pData = pNewData;
		capacity = desiredCapacity;
	}

	void push_back(const type& value) {
		if (count == capacity) {
			reserve(grow_capacity());
		}
		memcpy(&pData[count], &value, sizeof(type));
		count++;
	}

	void pop_back() {
		DEBUG_CHECK(count > 0);
		count--;
	}

	type& operator[](uint32 i) {
		DEBUG_CHECK(i >= 0 && i < count);
		return pData[i];
	}
	
	const type& operator[](uint32 i) const {
		DEBUG_CHECK(i >= 0 && i < count);
		return pData[i];
	}

	void clear() {
		free(pData);
		pData = nullptr;
		count = 0;
		capacity = 0;
	}

	void erase(uint32 index) {
		DEBUG_CHECK(index >= 0 && index < count);
		if (index == count-1) {
			pop_back(); count--;
		}
		if (index < count-1) {
			memmove(pData + index, pData + (index + 1), (count - index) * sizeof(type));
			count--;
		}
	}

	void erase_unsorted(uint32 index) {
		DEBUG_CHECK(index >= 0 && index < count);
		if (index == count-1) {
			pop_back(); count--;
		}
		if (index < count-1) {
			memcpy(pData + index, pData + (count - 1), sizeof(type));
			count--;
		}
	}
	
	void insert(uint32 index, const type& value) {
		DEBUG_CHECK(index >= 0 && index < count);
		if (capacity == count) reserve(grow_capacity());
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

	uint32 index_from_ptr(const type* ptr) const {
		DEBUG_CHECK(ptr >= pData && ptr < pData + count);
		ptrdiff_t diff = ptr - pData;
		return (uint32)diff;
	}

	bool validate() const {
		return capacity >= count;
	}

private:
	uint32 grow_capacity() {
		uint32 newCapacity;
		if (capacity == 0)
			newCapacity = 8;
		else
			newCapacity = 2 * capacity;
		return newCapacity;
	}
};



// ----------------------
// Test Helpers
// ----------------------

void StartTest(const char* testName) {
	printf("Starting test: %s\n", testName);
}

void EndTest(int errorCount) {
	if (errorCount == 0) {
		printf("PASS\n\n");
	} else {
		printf("Failed with %i errors\n", errorCount);
	}
	return;
}

bool _verify(bool expression, int& errorCount, const char* file, int line, const char* msg) {
	if (!expression) {
		errorCount++;
		printf("FAIL: %s(%i): %s\n", file, line, msg);
		return false;
	}
	return true;
}

#define VERIFY(expression) _verify((expression), errorCount, __FILE__, __LINE__, (#expression))
#define VERIFY_MSG(expression, msg) _verify((expression), errorCount, __FILE__, __LINE__, msg)





// ---------------------
// Tests
// ---------------------

int arrayTest() {
	StartTest("Array Test");
	int errorCount = 0;

	Array<int> testArray;

	// Reserve memory
	testArray.reserve(2);
	VERIFY(testArray.capacity == 2);
	
	// Appending values
	testArray.push_back(1337);
	testArray.push_back(420);
	testArray.push_back(1800);
	testArray.push_back(77);
	testArray.push_back(99);
	VERIFY(testArray[0] == 1337);
	VERIFY(testArray[1] == 420);
	VERIFY(testArray[2] == 1800);
	VERIFY(testArray[3] == 77);
	VERIFY(testArray[4] == 99);
	VERIFY(testArray.count == 5);
	VERIFY(testArray.capacity == 8);

	// Reserve should memcpy the old data to the new reserved location 
	testArray.reserve(50);
	VERIFY(testArray[2] == 1800);
	VERIFY(testArray.capacity == 50);
	VERIFY(testArray.count == 5);
	
	// Iteration
	int sum = 0;
	for (int i : testArray) {
		sum += i;
	}
	VERIFY(sum == 3733);

	// Erasing elements
	testArray.erase(2);
	testArray.erase_unsorted(0);
	VERIFY(testArray[0] == 99);
	VERIFY(testArray[1] == 420);
	VERIFY(testArray[2] == 77);
	VERIFY(testArray.count == 3);

	// Remove element at back
	testArray.pop_back();
	VERIFY(testArray.count == 2);

	// Inserting elements
	testArray.insert(0, 9999);
	testArray.insert(1, 1111);
	testArray.insert(1, 2222);
	VERIFY(testArray[0] == 9999);
	VERIFY(testArray[1] == 2222);
	VERIFY(testArray[2] == 1111);
	VERIFY(testArray[3] == 99);
	VERIFY(testArray[4] == 420);
	VERIFY(testArray.count == 5);

	// Finding elements
	VERIFY(testArray.index_from_ptr(testArray.find(99)) == 3);
	VERIFY(testArray.index_from_ptr(testArray.find(1111)) == 2);
	VERIFY(testArray.find(1337) == testArray.end());

	// Clear the array
	testArray.clear();
	VERIFY(testArray.count == 0);
	VERIFY(testArray.capacity == 0);
	VERIFY(testArray.pData == nullptr);

	// You wanna test for mem leaks at the end of this function
	// Plus test for memory overwrites and so on (tests should maybe be done in address sanitizer mode)

	EndTest(errorCount);
    return 0;
}  

// TODO: 
// 1. Make a dynamic array class ------ DONE
// 2. Install VS2022 and try out address sanitizer
// 3. Make a string view class
// 4. Make a dynamic string class inspired by bx stringT and built on the dynamic array 
// 5. Make a simple hash map class
// 6. Override global allocation functions to do memory tracking for debugging (new, delete, alloc, free)
// 7. Make a custom allocator mechanism for the dynamic array class
// 8. Make linear pool allocator


// Jai's Pool allocator
// Linked list of blocks basically, allocates a new one if you try get memory more than is free in the existing block


// Re: Allocators (point 2)
//
// I have the feeling that a virtual function allocator interface is actually going to be fine
// So we can avoid the template allocator parameter. And just have set/get allocator functions on containers
// Things that will free much later than they alloc must store the allocator for later freeing
// Everything else that does mem alloc should take an allocator as a parameter to make it clear they're doing mem ops
// I think we should have a global mem allocator object that anyone can access
// Regarding global context stack, we can probably play with this as a bonus if needed, but lets see how far we get without it first please


// Re: Memory debug tracking
//
// Memory debugger.jai is a good example here.
// on alloc we add the allocation to a table
// on free, we look for the allocation in the table


int main() {
	arrayTest();
	__debugbreak();
    return 0;
}