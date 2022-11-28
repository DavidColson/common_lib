#include "string.h"
#include "string_builder.h"
#include "hashmap.h"
#include "resizable_array.h"
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
#define defer(code)   auto DEFER_3(_defer_) = defer_func([&]() { code; })

// Hash Node used in hashmap below
// -------------------------------

#define UNUSED_HASH 0
#define FIRST_VALID_HASH 1

template<typename K, typename V>
struct HashNode2 {
	uint64_t hash { UNUSED_HASH };
	K key;
	V value;
};

template<typename K, typename V, typename AllocatorType = Allocator, typename KF = KeyFuncs<K>>
struct HashMap2 {
	HashNode2<K, V>* pTable { nullptr };
	KF keyFuncs;
	size_t tableSize { 0 };
	size_t count { 0 };
	AllocatorType allocator;

	void Free() {
		allocator.Free(pTable);
	}

	template<typename F>
	void Free(F&& freeNode) {
		for (size_t i = 0; i < tableSize; i++) {
			if (pTable[i] != UNUSED_HASH) {
				freeNode(pTable[i]);
			}
		}
		allocator.Free(pTable);
	}

	V& Add(const K& key, const V& value) {
		float loadFactor = tableSize == 0 ? INT_MAX : (float)count / (float)tableSize;
		if (loadFactor >= 1.0f) Rehash(tableSize + 1);

		uint64_t hash = keyFuncs.Hash(key);
		if (hash < FIRST_VALID_HASH) hash += FIRST_VALID_HASH;
		uint64_t index = hash % tableSize;
		uint64_t probeCounter = 1;

		while (pTable[index].hash != UNUSED_HASH) {
			index = (index + probeCounter) % tableSize;
			probeCounter++;
		}

		HashNode2<K, V>& node = pTable[index];
		node.hash = hash;
		node.key = key;
		node.value = value;
		count++;
		return node.value;
	}

	V* Get(const K& key) {
		if (tableSize == 0) return nullptr;

		uint64_t hash = keyFuncs.Hash(key);
		if (hash < FIRST_VALID_HASH) hash += FIRST_VALID_HASH;
		uint64_t index = hash % tableSize;
		uint64_t probeCounter = 1;

		while (pTable[index].hash != UNUSED_HASH) {
			if (keyFuncs.Cmp(pTable[index].key, key)) {
				return &pTable[index].value;
			}
			index = (index + probeCounter) % tableSize;
			probeCounter++;
		}
		return nullptr;
	}

	V& GetOrAdd(const K& key) {
		if (tableSize == 0) {
			V value = V();
			return Add(key, value);
		}

		uint64_t hash = keyFuncs.Hash(key);
		if (hash < FIRST_VALID_HASH) hash += FIRST_VALID_HASH;
		uint64_t index = hash % tableSize;
		uint64_t probeCounter = 1;

		while (pTable[index].hash != UNUSED_HASH) {
			if (keyFuncs.Cmp(pTable[index].key, key)) {
				return pTable[index].value;
			}
			index = (index + probeCounter) % tableSize;
			probeCounter++;
		}
		V value = V();
		return Add(key, value);
	}

	V& operator[](const K& key) {
		return GetOrAdd(key);
	}

	void Erase(const K& key) {
		Erase(key, [](HashNode2<K, V>) {});
	}
	
	template<typename F>
	void Erase(const K& key, F&& freeNode) {
		if (tableSize == 0) return;

		uint64_t hash = keyFuncs.Hash(key);
		if (hash < FIRST_VALID_HASH) hash += FIRST_VALID_HASH;
		uint64_t index = hash % tableSize;
		uint64_t probeCounter = 1;

		while (pTable[index].hash != UNUSED_HASH) {
			if (keyFuncs.Cmp(pTable[index].key, key)) {
				// Found the node
				freeNode(pTable[index]);
				memset(&pTable[index], 0, sizeof(HashNode2<K, V>));
				count--;
			}
			index = (index + probeCounter) % tableSize;
			probeCounter++;
		}
	}

	void Rehash(size_t requiredTableSize) {
		if (requiredTableSize < tableSize)
			return;

		HashNode2<K, V>* pTableOld = pTable;

		// Double the table size until we can fit required table size
		constexpr size_t minTableSize = 32;
		size_t newTableSize = tableSize == 0 ? minTableSize : tableSize * 2;
		while (newTableSize < (requiredTableSize > minTableSize ? requiredTableSize : minTableSize))
			newTableSize *= 2;

		pTable = (HashNode2<K, V>*)allocator.Allocate(newTableSize * sizeof(HashNode2<K, V>));
		memset(pTable, 0, newTableSize * sizeof(HashNode<K, V>));

		size_t oldTableSize = tableSize;
		tableSize = newTableSize;
		for (int i = 0; i < oldTableSize; i++) {
			if (pTableOld[i].hash != UNUSED_HASH) {
				Add(pTableOld[i].key, pTableOld[i].value);
				count--;
			}
		}
		if (pTableOld) allocator.Free(pTableOld);
	}
};

// ---------------------
// Tests
// ---------------------

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

int HashMap2Test() {
	StartTest("HashMap2 Test");
	int errorCount = 0;
	{
		HashMap2<int, int> testMap;
		defer(testMap.Free());

		testMap.Add(1337, 22);
		testMap.Add(52, 21);
		testMap.Add(87, 20);
		testMap.Add(12, 19);
		testMap.Add(18832, 18);
		testMap.Add(875, 17);
		testMap.Add(123, 16);
		testMap.Add(9812, 15);
		VERIFY(testMap.tableSize == 32);
		VERIFY(testMap.count == 8);

		VERIFY(*testMap.Get(1337) == 22);
		VERIFY(*testMap.Get(52) == 21);
		VERIFY(*testMap.Get(87) == 20);
		VERIFY(*testMap.Get(12) == 19);
		VERIFY(*testMap.Get(18832) == 18);
		VERIFY(*testMap.Get(875) == 17);
		VERIFY(*testMap.Get(123) == 16);
		VERIFY(*testMap.Get(9812) == 15);

		testMap.Erase(123);
		testMap.Erase(12);
		testMap.Erase(87);
		VERIFY(testMap.count == 5);
		VERIFY(testMap.Get(123) == nullptr);
		VERIFY(testMap.Get(12) == nullptr);
		VERIFY(testMap.Get(87) == nullptr);

		HashMap2<String, int> testMap2;
		defer(testMap2.Free());
		
		testMap2.GetOrAdd("Dave") = 27;
		testMap2.GetOrAdd("Lucy") = 27;
		testMap2["Jonny"] = 28;
		testMap2["Mark"] = 30;
		VERIFY(testMap2.tableSize == 32);
		VERIFY(testMap2.count == 4);

		VERIFY(testMap2["Dave"] == 27);
		VERIFY(testMap2["Lucy"] == 27);
		VERIFY(testMap2["Jonny"] == 28);
		VERIFY(testMap2["Mark"] == 30);

		// Test rehashing
		HashMap2<int, int> testMap3;
		defer(testMap3.Free());
		
		srand(7);
		for (int i = 100; i > 0; i--) {
			testMap3.Add(rand(), i);
		}
		VERIFY(testMap3[6280] == 37);
		VERIFY(testMap3[10780] == 4);
		VERIFY(testMap3[9087] == 13);
		VERIFY(testMap3[9185] == 79);
		VERIFY(testMap3.tableSize == 128);
		VERIFY(testMap3.count == 100);

		// Testing some other common key types		
		HashMap2<float, int> floatMap;
		floatMap[28.31f] = 1337;
		floatMap[4.1231f] = 1338;
		floatMap[0.78f] = 1339;
		VERIFY(floatMap[28.31f] == 1337);
		VERIFY(floatMap[4.1231f] == 1338);
		VERIFY(floatMap[0.78f] == 1339);
		floatMap.Free();

		HashMap2<char, int> charMap;
		charMap['c'] = 1337;
		charMap['8'] = 1338;
		charMap['U'] = 1339;
		VERIFY(charMap['c'] == 1337);
		VERIFY(charMap['8'] == 1338);
		VERIFY(charMap['U'] == 1339);
		charMap.Free();

		HashMap2<const char*, int> cStringMap;
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

		HashMap2<int*, int> pointerMap;
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

		HashMap2<CustomKeyType, int> customMap;
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

int StringTest() {
	StartTest("String Test");
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

int ResizableArrayTest() {
	StartTest("ResizableArray Test");
	int errorCount = 0;
	{
		ResizableArray<int> testArray;

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
// [x] Array becomes resizeable array (that is not automatically freed)
// [ ] Rewrite hashmap to be open addressing table and to not support non-POD data types
// [ ] Mem tracker spits out error messages for Unknown Memory and Double free memory
// [ ] Mem tracker spits out error messages for mismatched allocator
// [ ] Container constructors allow runtime allocator assignment
// [ ] Make linear pool allocator
// [ ] Experiment with memory alignment in our custom allocators (i.e. perf before after alignment)


// Jai's Pool allocator
// Linked list of blocks basically, allocates a new one if you try get memory more than is free in the existing block



int main() {
	HashMap2Test();
	StringTest();
	ResizableArrayTest();
	__debugbreak();
    return 0;
}