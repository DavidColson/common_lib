
// TODO: put this someplace sensible
#ifdef _DEBUG
#define DEBUG_CHECK(expression) if (!(expression)) __debugbreak();
#else
#define DEBUG_CHECK(expression)
#endif

#include "hashmap.h"
#include "string_view.h"
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
		badCopy.AppendChars(&c, 1);
	}
	VERIFY(strcmp(badCopy.pData, "Hello world my name is David and I wrote this code") == 0);
	VERIFY(badCopy.length == 50);
	VERIFY(badCopy.capacity == 64);

	// Comparison
	String comparisonTest = "David";
	VERIFY(comparisonTest == "David");
	VERIFY(comparisonTest != "David Colson");
	VERIFY(comparisonTest != "Dav");

	String comparisonTest2 = "David";
	String comparisonTest3 = "David Colson";
	VERIFY(comparisonTest == comparisonTest2);
	VERIFY(comparisonTest != comparisonTest3);

	EndTest(errorCount);
	return 0;
}

bool StringViewParameterTest(StringView stringView, const char* comparison)
{
	if (strcmp(stringView.pData, comparison) == 0 && stringView.length == strlen(comparison))
		return true;
	return false;
}

int StringViewTest() {
	StartTest("StringView Test");
	int errorCount = 0;

	StringView simpleView("Hello World");
	VERIFY(strcmp(simpleView.pData, "Hello World") == 0);
	VERIFY(simpleView.length == 11);

	simpleView = "Hello Internet";
	VERIFY(strcmp(simpleView.pData, "Hello Internet") == 0);
	VERIFY(simpleView.length == 14);

	// Can pass const chars to functions expecting string views
	VERIFY(StringViewParameterTest("Hello world", "Hello world"));

	// Can pass normal strings to functions expecting string views
	String testString = "Geese are scary";
	VERIFY(StringViewParameterTest(testString, "Geese are scary"));

	// TODO string scanning functions (i.e. advance, peek, is digit etc etc) on string views
	
	// Comparison operators?
	// Advance on string view (increments ptr)
	// Peek (returns next character)
	// Advance whitespace
	// Match
	// IsDigit
	// IsAlpha
	// IsHexDigit
	// IsAlphaNumeric

	EndTest(errorCount);
	return 0;
}

int HashMapTest() {
	StartTest("HashMap Test");
	int errorCount = 0;

	HashMap<String, int> ageMap;

	ageMap["Dave"] = 27;
	VERIFY(ageMap["Dave"] == 27);

	ageMap["Szymon"] = 28;
	VERIFY(ageMap["Dave"] == 27);
	VERIFY(ageMap["Szymon"] == 28);
	VERIFY(ageMap.size == 2);
	VERIFY(ageMap.bucketCount == 11);

	ageMap["Dave"] = 29;
	VERIFY(ageMap["Dave"] == 29);

	// Check tells you if an item exists, and gets it for you if it does
	int daveAge;
	VERIFY(ageMap.Check("Dave", daveAge));
	VERIFY(daveAge == 29);

	// Rehashing and resizing the table
	ageMap.Rehash(2);
	VERIFY(ageMap.LoadFactor() == 1.0);
	VERIFY(ageMap.size == 2);
	VERIFY(ageMap.bucketCount == 2);

	ageMap["Jonny"] = 31;
	ageMap["Mark"] = 32;
	VERIFY(ageMap.size == 4);
	VERIFY(ageMap.bucketCount == 5);

	VERIFY(ageMap.LoadFactor() == 4.f/5.f);
	ageMap.Rehash(8);
	VERIFY(ageMap.LoadFactor() == 0.5);
	VERIFY(ageMap["Dave"] == 29);
	VERIFY(ageMap["Szymon"] == 28);
	VERIFY(ageMap["Jonny"] == 31);
	VERIFY(ageMap["Mark"] == 32);

	VERIFY(ageMap.GetIdealBucketCount(12) == 13);
	VERIFY(ageMap.GetIdealBucketCount(8) == 11);
	VERIFY(ageMap.GetIdealBucketCount(5) == 7);
	VERIFY(ageMap.GetIdealBucketCount(25) == 29);

	// Erase Elements
	ageMap.Erase("Dave");
	VERIFY(!ageMap.Check("Dave", daveAge));
	VERIFY(ageMap["Szymon"] == 28);
	VERIFY(ageMap["Jonny"] == 31);
	VERIFY(ageMap["Mark"] == 32);

	// Checking erasing when items are in bucket linked lists
	ageMap["Dave"] = 27;
	ageMap["Chris"] = 25;
	ageMap.Rehash(2);
	ageMap.Erase("Chris");
	ageMap.Erase("Szymon");
	VERIFY(!ageMap.Check("Chris", daveAge));
	VERIFY(!ageMap.Check("Szymon", daveAge));
	VERIFY(ageMap["Dave"] == 27);
	VERIFY(ageMap["Jonny"] == 31);
	VERIFY(ageMap["Mark"] == 32);
	VERIFY(ageMap.size == 3);
	VERIFY(ageMap.bucketCount == 2);

	// TODO: 
	// Tests for different kinds of keys (ints, floats, stringview?)
	// Test for custom key type

	// How tf does one make a hash map?
	// https://aozturk.medium.com/simple-hash-map-hash-table-implementation-in-c-931965904250
	// https://www.softwaretestinghelp.com/hash-table-cpp-programs/
	
	// https://en.cppreference.com/w/cpp/container/unordered_map
	// 
	// Interesting notes from c++ unordered map
	// There are functions for load balancing and improving performance
	// Load factor, showing average number of elements per bucket (closer to 1 the better the performance)
	// Provides a reserve function, which will ensure there are enough buckets for all your elements (keeping load factor low)
	// Rehashing
	// Note stl has a max load factor of 1.0 (elems/number of buckets)
	// When we insert a new element we check the load factor, if it's gone above the max (1.0) then we grow the bucket array
	// as per normal (doubling?) and rehash the whole table
	

	// https://github.com/electronicarts/EASTL/blob/master/include/EASTL/internal/hashtable.h
	//
	// Interesting note is that EASTL uses some prime number magic when deciding the new bucket count when rehashing
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
// [x] Make a string view class
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
	StringViewTest();
	HashMapTest();
	__debugbreak();
    return 0;
}