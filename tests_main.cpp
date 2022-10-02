
#include "stdio.h"
#include "stdlib.h"
#include "string.h"


// Core Types
typedef unsigned int uint32;


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

	void append(const type& value) {
		if (count == capacity) {
			reserve(grow_capacity());
		}
		memcpy(&pData[count], &value, sizeof(type));
		count++;
	}

	// TODO: Erase, Insert, iterator type

	type& operator[](int i) {
		return pData[i];
	}
	
	const type& operator[](int i) const {
		return pData[i];
	}

	bool validate() {
		return capacity >= count;
	}

private:
	uint32 grow_capacity() {
		uint32 newCapacity;
		if (capacity == 0)
			newCapacity = 8;
		else
			newCapacity = capacity + capacity / 2;
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

int arrayTest() {
	StartTest("Array Test");
	int errorCount = 0;

	Array<int> testArray;

	testArray.reserve(2);
	// TODO: Could verify the amount of memory requested from allocator?
	VERIFY(testArray.capacity == 2);
	
	testArray.append(1337);
	testArray.append(420);
	testArray.append(1800);
	testArray.append(77);
	testArray.append(99);
	VERIFY(testArray[0] == 1337);
	VERIFY(testArray[1] == 420);
	VERIFY(testArray[2] == 1800);
	VERIFY(testArray[3] == 77);
	VERIFY(testArray[4] == 99);
	VERIFY(testArray.count == 5);
	VERIFY(testArray.capacity == 6);

	// Reserve should memcpy the old data to the new reserved location
	testArray.reserve(50);
	VERIFY(testArray[2] == 1800);
	VERIFY(testArray.capacity == 50);
	VERIFY(testArray.count == 5);
	
	// You wanna test for mem leaks at the end of this function
	// Plus test for memory overwrites and so on (tests should maybe be done in address sanitizer mode)

	EndTest(errorCount);
    return 0;
}  

// TODO: 
// 1. Make a dynamic array class
// 2. Make a custom allocator mechanism for the dynamic array class
// 3. Make a dynamic string class inspired by bx stringT and built on the dynamic array 
// 4. Make a string view class
// 5. Make a simple hash map class
// 6. Make a memory tracker using custom allocator
// 7. Make other allocators (such as jai's pool alloc)


// Re: Allocators (point 2)
//
// I have the feeling that a virtual function allocator interface is actually going to be fine
// So we can avoid the template allocator parameter. And just have set/get allocator functions on containers
// Things that will free much later than they alloc must store the allocator for later freeing
// Everything else that does mem alloc should take an allocator as a parameter to make it clear they're doing mem ops
// I think we should have a global mem allocator object that anyone can access
// Regarding global context stack, we can probably play with this as a bonus if needed, but lets see how far we get without it first please


// Re: Tests
// https://github.com/maghoff/jsonxx/tree/master/tests

int main() {
	arrayTest();
    printf("hello world");
    return 0;
}