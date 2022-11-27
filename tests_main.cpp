#include "string.h"
#include "hashmap.h"
#include "array.h"
#include "testing.h"



// Find a place for this to live
template <typename F>
struct privDefer {
	F f;
	privDefer(F f) : f(f) {}
	~privDefer() { f(); }
};

template <typename F>
privDefer<F> defer_func(F f) {
	return privDefer<F>(f);
}

#define DEFER_1(x, y) x##y
#define DEFER_2(x, y) DEFER_1(x, y)
#define DEFER_3(x)    DEFER_2(x, __COUNTER__)
#define defer(code)   auto DEFER_3(_defer_) = defer_func([&](){code;})


// Non-null terminated string
// The functions in this struct guarantee to do zero memory operations
// and is fully POD, trivially copyable

// TODO: Move
template<>
struct KeyFuncs<String> {
	uint64_t Hash(const String& key) const
	{
		size_t nChars = key.length;
		size_t hash = 0x811C9DC5;
		const unsigned char* pData = (const unsigned char*)key.pData;
		while (nChars--)
			hash = (*pData++ ^ hash) * 0x01000193;
		return hash;
	}
	bool Cmp(const String& key1, const String& key2) const {
		return key1 == key2;
	}
};

template<typename AllocatorType = Allocator>
struct StringBuilder {
	char* pData = nullptr;
	uint32_t length = 0;
	uint32_t capacity = 0;
	AllocatorType allocator;

	StringBuilder() {}

	void AppendChars(const char* str, uint32_t len) {
		Reserve(GrowCapacity(length + len));
		memcpy(pData + length, str, len);
		length += len;
	}

	void Append(const char* str) {
		uint32_t addedLength = (uint32_t)strlen(str);
		AppendChars(str, addedLength);
	}

	void Append(String str) {
		AppendChars(str.pData, str.length);
	}

	void AppendFormatInternal(const char* format, va_list args) {
		va_list argsCopy;
		va_copy(argsCopy, args);

		int addedLength = vsnprintf(nullptr, 0, format, args);
		if (addedLength <= 0) {
			va_end(argsCopy);
			return;
		}

		Reserve(GrowCapacity(length + addedLength));
		vsnprintf(pData + length, addedLength + 1, format, args);
		va_end(argsCopy);
		length += addedLength;
	}

	void AppendFormat(const char* format, ...) {
		va_list args;
		va_start(args, format);
		AppendFormatInternal(format, args);
		va_end(args);
	}

	String CreateString(bool reset = true) {
		String output = AllocString(length, allocator);
		memcpy(output.pData, pData, length * sizeof(char));
		output.length = length;

		if (reset) Reset();
		return output;
	}

	void Reset() {
		allocator.Free(pData);
		pData = nullptr;
		length = 0;
		capacity = 0;
	}

	void Reserve(uint32_t desiredCapacity) {
		if (capacity >= desiredCapacity) return;
		pData = (char*)allocator.Reallocate(pData, desiredCapacity * sizeof(char));
		capacity = desiredCapacity;
	}

	uint32_t GrowCapacity(uint32_t atLeastSize) const {
		// if we're big enough already, don't grow, otherwise double, 
		// and if that's not enough just use atLeastSize
		if (capacity > atLeastSize) return capacity;
		uint32_t newCapacity = capacity ? capacity * 2 : 8;
		return newCapacity > atLeastSize ? newCapacity : atLeastSize;
	}
};

// ---------------------
// Tests
// ---------------------

int StringExperimentalTest() {
	StartTest("Experimental Strings");
	int errorCount = 0;
	{
		String emptyStr;
		VERIFY(emptyStr.pData == nullptr);
		VERIFY(emptyStr.length == 0);

		String str("Hello World");
		VERIFY(str == "Hello World");
		VERIFY(str.length == 11);

		str = "Hi Dave";
		VERIFY(str == "Hi Dave");
		VERIFY(str != "Hello World");
		VERIFY(str.length == 7);

		String copy = CopyCString("Ducks are cool");
		defer(FreeString(copy));
		VERIFY(copy == "Ducks are cool");
		VERIFY(copy.length == 14);

		String copy2 = CopyString(str);
		defer(FreeString(copy2));
		VERIFY(copy2 == "Hi Dave");
		VERIFY(copy2.length == 7);

		String allocated = AllocString(copy.length * sizeof(char));
		defer(FreeString(allocated));
		memcpy(allocated.pData, copy.pData, copy.length * sizeof(char));
		VERIFY(allocated == copy);
		VERIFY(allocated != str);
		VERIFY(allocated.length == 14);

		// String Builder, for dynamically constructing strings
		StringBuilder builder;
		builder.Append("Hello world");
		builder.AppendFormat(" my name is %s", "David");
		builder.AppendChars(" and this is my code bloop", 20);

		String builtString = builder.CreateString();
		defer(FreeString(builtString));
		VERIFY(builtString == "Hello world my name is David and this is my code");
		VERIFY(builtString != "Ducks");
		VERIFY(builtString.length == 48);
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

		ageMap.Free();

		// Testing a few common key types
		HashMap<int, int> intMap;
		intMap[2] = 1337;
		intMap[17] = 1338;
		intMap[6] = 1339;
		VERIFY(intMap[2] == 1337);
		VERIFY(intMap[17] == 1338);
		VERIFY(intMap[6] == 1339);
		intMap.Free();

		HashMap<float, int> floatMap;
		floatMap[28.31f] = 1337;
		floatMap[4.1231f] = 1338;
		floatMap[0.78f] = 1339;
		VERIFY(floatMap[28.31f] == 1337);
		VERIFY(floatMap[4.1231f] == 1338);
		VERIFY(floatMap[0.78f] == 1339);
		floatMap.Free();

		HashMap<char, int> charMap;
		charMap['c'] = 1337;
		charMap['8'] = 1338;
		charMap['U'] = 1339;
		VERIFY(charMap['c'] == 1337);
		VERIFY(charMap['8'] == 1338);
		VERIFY(charMap['U'] == 1339);
		charMap.Free();

		HashMap<const char*, int> cStringMap;
		cStringMap["Ducks"] = 1337;
		cStringMap["Cars"] = 1338;
		cStringMap["Hats"] = 1339;
		VERIFY(cStringMap["Ducks"] == 1337);
		VERIFY(cStringMap["Cars"] == 1338);
		VERIFY(cStringMap["Hats"] == 1339);
		cStringMap.Free();

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
		pointerMap.Free();

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
		customMap.Free();
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
		testArray.Free();

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
// [x] string builder class
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
	HashMapTest();
	__debugbreak();
    return 0;
}