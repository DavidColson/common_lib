#include "hashmap.h"
#include "string_view.h"
#include "string.h"
#include "array.h"
#include "testing.h"


struct MemString {
	char* pData = nullptr;
	size_t length = 0;

	// TODO
	// Comparison operators
	MemString() {}
	
	MemString(const char* str) {
		pData = const_cast<char*>(str);
		length = strlen(str);
	}

	void operator=(const char* str) {
		pData = const_cast<char*>(str);
		length = strlen(str);
	}

	bool operator==(MemString& other) {
		return strcmp(pData, other.pData) == 0;
	}

	bool operator!=(MemString& other) {
		return strcmp(pData, other.pData) != 0;
	}
};

template<typename AllocatorType = Allocator>
MemString CopyCString(const char* string, AllocatorType allocator = Allocator()) {
	MemString s;
	size_t len = strlen(string);
	s.pData = (char*)allocator.Allocate((len+1) * sizeof(char));
	s.length = len;
	memcpy(s.pData, string, (len+1) * sizeof(char));
	return s;
}

template<typename AllocatorType = Allocator>
MemString CopyString(MemString& string, AllocatorType allocator = Allocator()) {
	MemString s;
	s.pData = (char*)allocator.Allocate((string.length+1) * sizeof(char));
	s.length = string.length;
	memcpy(s.pData, string.pData, (string.length+1) * sizeof(char));
	return s;
}

template<typename AllocatorType = Allocator>
MemString AllocString(size_t length, AllocatorType allocator = Allocator()) {
	MemString s;
	s.pData = (char*)allocator.Allocate(length * sizeof(char));
	s.length = length;
	return s;
}

template<typename AllocatorType = Allocator>
void FreeString(MemString& string, AllocatorType allocator = Allocator()) {
	allocator.Free(string.pData);
	string.pData = nullptr;
	string.length = 0;
}

// ---------------------
// Tests
// ---------------------

int StringExperimentalTest() {

	// Goal, we want a string that is POD, can be memcpy'd easily
	// Manually memory managed and tracked by memory tracker

	// How about the core string class does zero memory allocations
	// We provide some external functions which can copy a string
	// And we provide a string builder struct which takes an allocator,
	// which will do the grow capacity stuff as normal on append
	// string builder is where append format lives as well and any other creation utilities

	// TODO: 
	// Write tests for all the above copy, allocation and free functions
	// Write tests for assignment and equality functions

	MemString copy;
	{
		const char* cstring = "Hello World";

		MemString str("Hello World 2");
		str = cstring;
		str = "Ducks";
		copy = str;

		copy = CopyCString("Test String");
	}

	FreeString(copy);

	ReportMemoryLeaks();
	return 0;
}

int StringTest() {
	StartTest("String Test");
	int errorCount = 0;
	{
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
	}

	errorCount += ReportMemoryLeaks();
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
	{
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
	}
	errorCount += ReportMemoryLeaks();
	EndTest(errorCount);
	return 0;
}

struct CustomKeyType {
	int thing;
	int thing2;
};

// Your custom type must define KeyFuncs to be able to used as a key
template<>
struct KeyFuncs<CustomKeyType> {
	uint64_t Hash(const CustomKeyType& key) const
	{
		// This is probably not a good hash method for this type
		// But it serves for demonstration
		return static_cast<uint64_t>(key.thing + key.thing2);
	}
	bool Cmp(const CustomKeyType& key1, const CustomKeyType& key2) const {
		return key1.thing == key2.thing && key1.thing2 == key2.thing2;
	}
};

int HashMapTest() {
	StartTest("HashMap Test");
	int errorCount = 0;
	{
		HashMap<String, int> ageMap;

		// Use operator [] to access and add elements
		ageMap["Dave"] = 27;
		VERIFY(ageMap["Dave"] == 27);

		ageMap["Szymon"] = 28;
		VERIFY(ageMap["Dave"] == 27);
		VERIFY(ageMap["Szymon"] == 28);
		VERIFY(ageMap.size == 2);
		VERIFY(ageMap.bucketCount == 11);

		ageMap["Dave"] = 29;
		VERIFY(ageMap["Dave"] == 29);

		// Exists tells you if an item exists
		VERIFY(ageMap.Exists("Dave"));

		// Rehashing and resizing the table
		ageMap.Rehash(2);
		VERIFY(ageMap.LoadFactor() == 1.0);
		VERIFY(ageMap.size == 2);
		VERIFY(ageMap.bucketCount == 2);

		ageMap["Jonny"] = 31;
		ageMap["Mark"] = 32;
		VERIFY(ageMap.size == 4);
		VERIFY(ageMap.bucketCount == 5);

		VERIFY(ageMap.LoadFactor() == 4.f / 5.f);
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
		VERIFY(!ageMap.Exists("Dave"));
		VERIFY(ageMap["Szymon"] == 28);
		VERIFY(ageMap["Jonny"] == 31);
		VERIFY(ageMap["Mark"] == 32);

		// Checking erasing when items are in bucket linked lists
		ageMap["Dave"] = 27;
		ageMap["Chris"] = 25;
		ageMap.Rehash(2);
		ageMap.Erase("Chris");
		ageMap.Erase("Szymon");
		VERIFY(!ageMap.Exists("Chris"));
		VERIFY(!ageMap.Exists("Szymon"));
		VERIFY(ageMap["Dave"] == 27);
		VERIFY(ageMap["Jonny"] == 31);
		VERIFY(ageMap["Mark"] == 32);
		VERIFY(ageMap.size == 3);
		VERIFY(ageMap.bucketCount == 2);

		// Testing a few common key types
		HashMap<int, int> intMap;
		intMap[2] = 1337;
		intMap[17] = 1338;
		intMap[6] = 1339;
		VERIFY(intMap[2] == 1337);
		VERIFY(intMap[17] == 1338);
		VERIFY(intMap[6] == 1339);

		HashMap<float, int> floatMap;
		floatMap[28.31f] = 1337;
		floatMap[4.1231f] = 1338;
		floatMap[0.78f] = 1339;
		VERIFY(floatMap[28.31f] == 1337);
		VERIFY(floatMap[4.1231f] == 1338);
		VERIFY(floatMap[0.78f] == 1339);

		HashMap<char, int> charMap;
		charMap['c'] = 1337;
		charMap['8'] = 1338;
		charMap['U'] = 1339;
		VERIFY(charMap['c'] == 1337);
		VERIFY(charMap['8'] == 1338);
		VERIFY(charMap['U'] == 1339);

		HashMap<const char*, int> cStringMap;
		cStringMap["Ducks"] = 1337;
		cStringMap["Cars"] = 1338;
		cStringMap["Hats"] = 1339;
		VERIFY(cStringMap["Ducks"] == 1337);
		VERIFY(cStringMap["Cars"] == 1338);
		VERIFY(cStringMap["Hats"] == 1339);

		int arr[3];
		arr[0] = 3;
		arr[1] = 2;
		arr[2] = 1;

		HashMap<int*, int> pointerMap;
		pointerMap[arr] = 1337;
		pointerMap[arr + 1] = 1338;
		pointerMap[arr + 2] = 1339;
		VERIFY(pointerMap[arr] == 1337);
		VERIFY(pointerMap[arr + 1] == 1338);
		VERIFY(pointerMap[arr + 2] == 1339);

		// Custom Key Types
		CustomKeyType one { 1, 2 };
		CustomKeyType two { 4, 1 };
		CustomKeyType three { 3, 8 };

		HashMap<CustomKeyType, int> customMap;
		customMap[one] = 1337;
		customMap[two] = 1338;
		customMap[three] = 1339;
		VERIFY(customMap[one] == 1337);
		VERIFY(customMap[two] == 1338);
		VERIFY(customMap[three] == 1339);
	}
	errorCount += ReportMemoryLeaks();
	EndTest(errorCount);
	return 0;
}

int ArrayTest() {
	StartTest("Array Test");
	int errorCount = 0;
	{
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


		// TODO: test with strings and non pod stuff
	}
	errorCount += ReportMemoryLeaks();
	EndTest(errorCount);
    return 0;
}  

// Goals:
// No copy constructors
// No constructors, POD data everywhere
// Explicit memory operations for things

// TODO: 
// [x] Make a dynamic array class
// [x] Install VS2022 and try out address sanitizer
// [x] Change array to use realloc it's cool
// [x] Make a dynamic string class inspired by ImGuiTextBuffer and built on dyn array
// [x] Make a string view class
// [x] Make a simple hash map class
// [x] Make a custom allocator mechanism for commonLib
// [x] Create a memory tracker similar to jai which wraps around alloc/realloc/free for commonLib
// [x] New POD string types that do no memory operations
// [ ] string builder class
// [ ] Array becomes resizeable array (that is not automatically freed)
// [ ] Array type becomes a slice of another array (c style, or resizeable)
// [ ] Rewrite hashmap to be open addressing table and to not support non-POD data types
// [ ] Mem tracker spits out error messages for Unknown Memory and Double free memory
// [ ] Mem tracker spits out error messages for mismatched allocator
// [ ] Container constructors allow runtime allocator assignment
// [ ] Make linear pool allocator
// [ ] Experiment with memory alignment in our custom allocators (i.e. perf before after alignment)


// Jai's Pool allocator
// Linked list of blocks basically, allocates a new one if you try get memory more than is free in the existing block



int main() {
	StringExperimentalTest();
	ArrayTest();
	StringTest();
	StringViewTest();
	HashMapTest();
	__debugbreak();
    return 0;
}