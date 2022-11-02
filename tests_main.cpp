
#include "string.h"
#include "array.h"
#include "testing.h"

// ---------------------
// Tests
// ---------------------

int StringTest() {
	StartTest("String Test");
	int errorCount = 0;

	// Constructing Strings
	String myString;
	myString = "Hello World";
	VERIFY(strcmp(myString.pData, "Hello World") == 0);
	VERIFY(myString.length == 11);
	VERIFY(myString.capacity == 12);

	String mySecondString("Hello World 2");
	VERIFY(strcmp(mySecondString.pData, "Hello World 2") == 0);
	VERIFY(mySecondString.length == 13);
	VERIFY(mySecondString.capacity == 14);

	// Changing existing string to something else
	myString = "Hello world my name is David";
	VERIFY(strcmp(myString.pData, "Hello world my name is David") == 0);
	VERIFY(myString.length == 28);
	VERIFY(myString.capacity == 29);

	// Appending to the string
	myString.Append(" and I wrote this code");
	VERIFY(strcmp(myString.pData, "Hello world my name is David and I wrote this code") == 0);
	VERIFY(myString.length == 50);
	VERIFY(myString.capacity == 58);

	// Appending formatted strings
	String newString;
	newString.AppendFormat("Hello %s %f", "world", 5.12f);
	VERIFY(strcmp(newString.pData, "Hello world 5.120000") == 0);
	VERIFY(newString.length == 20);
	VERIFY(newString.capacity == 21);

	newString.AppendFormat(" there are %i %s", 2000, "people");
	VERIFY(strcmp(newString.pData, "Hello world 5.120000 there are 2000 people") == 0);
	VERIFY(newString.length == 42);
	VERIFY(newString.capacity == 43);

	// Clearing strings
	newString.Clear();
	VERIFY(newString.pData == nullptr);
	VERIFY(newString.length == 0);
	VERIFY(newString.capacity == 0);

	// Iterating characters
	String badCopy;
	for (char c : myString) {
		badCopy.Append(&c);
	}
	VERIFY(strcmp(badCopy.pData, "Hello world my name is David and I wrote this code") == 0);
	VERIFY(badCopy.length == 50);
	VERIFY(badCopy.capacity == 64);


	// TODO
	// [ ] string view struct (construct from String, String construct from view)
	// [ ] string scanning functions (i.e. advance, peek, is digit etc etc) on string views

	EndTest(errorCount);
	return 0;
}

int ArrayTest() {
	StartTest("Array Test");
	int errorCount = 0;

	Array<int> testArray;

	// Reserve memory
	testArray.Reserve(2);
	VERIFY(testArray.capacity == 2);
	
	// Appending values
	testArray.PushBack(1337);
	testArray.PushBack(420);
	testArray.PushBack(1800);
	testArray.PushBack(77);
	testArray.PushBack(99);
	VERIFY(testArray[0] == 1337);
	VERIFY(testArray[1] == 420);
	VERIFY(testArray[2] == 1800);
	VERIFY(testArray[3] == 77);
	VERIFY(testArray[4] == 99);
	VERIFY(testArray.count == 5);
	VERIFY(testArray.capacity == 8);

	// Reserve should memcpy the old data to the new Reserved location 
	testArray.Reserve(50);
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
	testArray.Erase(2);
	testArray.EraseUnsorted(0);
	VERIFY(testArray[0] == 99);
	VERIFY(testArray[1] == 420);
	VERIFY(testArray[2] == 77);
	VERIFY(testArray.count == 3);

	// Remove element at back
	testArray.PopBack();
	VERIFY(testArray.count == 2);

	// Inserting elements
	testArray.Insert(0, 9999);
	testArray.Insert(1, 1111);
	testArray.Insert(1, 2222);
	VERIFY(testArray[0] == 9999);
	VERIFY(testArray[1] == 2222);
	VERIFY(testArray[2] == 1111);
	VERIFY(testArray[3] == 99);
	VERIFY(testArray[4] == 420);
	VERIFY(testArray.count == 5);

	// Finding elements
	VERIFY(testArray.IndexFromPointer(testArray.Find(99)) == 3);
	VERIFY(testArray.IndexFromPointer(testArray.Find(1111)) == 2);
	VERIFY(testArray.Find(1337) == testArray.end());

	// Clear the array
	testArray.Clear();
	VERIFY(testArray.count == 0);
	VERIFY(testArray.capacity == 0);
	VERIFY(testArray.pData == nullptr);

	// You wanna test for mem leaks at the end of this function
	// Plus test for memory overwrites and so on (tests should maybe be done in address sanitizer mode)

	EndTest(errorCount);
    return 0;
}  

// TODO: 
// [x] Make a dynamic array class
// [x] Install VS2022 and try out address sanitizer
// [x] Change array to use realloc it's cool
// [x] Make a dynamic string class inspired by ImGuiTextBuffer and built on dyn array
// [ ] Make a string view class
// [ ] Make a simple hash map class
// [ ] Override global allocation functions to do memory tracking for debugging (new, delete, alloc, free)
// [ ] Make a custom allocator mechanism for the dynamic array class
// [ ] Make linear pool allocator


// Jai's Pool allocator
// Linked list of blocks basically, allocates a new one if you try get memory more than is free in the existing block


// Re: Allocators (point 2)
//
// I have the feeling that a virtual function allocator interface is actually going to be fine
// So we can avoid the template allocator parameter. And just have set/get allocator functions on containers
// Things that will free much later than they alloc must store the allocator for later freeing
// Everything else that does mem alloc should take an allocator as a parameter to make it Clear they're doing mem ops
// I think we should have a global mem allocator object that anyone can access
// Regarding global context stack, we can probably play with this as a bonus if needed, but lets see how far we get without it first please


// Re: Memory debug tracking
//
// Memory debugger.jai is a good example here.
// on alloc we add the allocation to a table
// on free, we look for the allocation in the table


int main() {
	ArrayTest();
	StringTest();
	__debugbreak();
    return 0;
}