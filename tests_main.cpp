
#include "json.h"
#include "linear_allocator.h"
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

int HashMapTest() {
	StartTest("HashMap Test");
	int errorCount = 0;
	{
		HashMap<int, int> testMap;
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

		HashMap<String, int> testMap2;
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
		HashMap<int, int> testMap3;
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
		testArray.Reset();
		VERIFY(testArray.count == 0);
		VERIFY(testArray.capacity == 0);
		testArray.Free();
		testArray.Free();
	}
	errorCount += ReportMemoryLeaks();
	EndTest(errorCount);
    return 0;
}  

void LinearAllocatorTest() {
	StartTest("LinearAllocator Test");
	int errorCount = 0;
	{
		LinearAllocator davesAllocator;

		// No freeing required!
		ResizableArray<int> myArray(&davesAllocator);
		for (int i = 0; i < 3000; i++) {
			myArray.PushBack(i);
		}

		HashMap<int, int> testMap(&davesAllocator);
		srand(7);
		for (int i = 100; i > 0; i--) {
			testMap.Add(rand(), i);
		}

		ResizableArray<String> myStringArray(&davesAllocator);
		for (int i = 0; i < 1000; i++) {
			StringBuilder builder(&davesAllocator);
			builder.AppendFormat("Hello %i", i);
			myStringArray.PushBack(builder.CreateString(true, &davesAllocator));
			myStringArray.PushBack(CopyCString("world", &davesAllocator));
		}
		
		davesAllocator.Finished();
	}
	errorCount += ReportMemoryLeaks();
	EndTest(errorCount);
}

void JsonTest() {
	StartTest("Json Test");
	int errorCount = 0;
	{
		const char* json = 
			"{\"widget\": {"
			"\"debug\": \"on\","
			"\"window\": {"
			"	\"title\": \"Sample Konfabulator Widget\","
			"	\"name\": \"main_window\","
			"	\"width\": 500,"
			"	\"height\": 500"
			"},"
			"\"image\": { "
			"	\"src\": \"Images/Sun.png\","
			"	\"name\": \"sun1\","
			"	\"hOffset\": 250,"
			"	\"vOffset\": 250,"
			"	\"alignment\": \"center\""
			"},"
			"\"text\": {"
			"	\"data\": \"Click Here\","
			"	\"size\": 36,"
			"	\"style\": \"bold\","
			"	\"name\": \"text1\","
			"	\"hOffset\": 250,"
			"	\"vOffset\": 100,"
			"	\"alignment\": \"center\","
			"	\"onMouseUp\": \"sun1.opacity = (sun1.opacity / 100) * 90;\""
			"}"
			"}}";

		String jsonString;
		jsonString = json;
		JsonValue v = ParseJsonFile(&gAllocator, jsonString);
		defer(v.Free());

		VERIFY(v["widget"]["debug"].ToString() == "on");
		VERIFY(v["widget"]["window"]["width"].ToInt() == 500);
		VERIFY(v["widget"]["image"]["src"].ToString() == "Images/Sun.png");
		VERIFY(v["widget"]["text"]["hOffset"].ToInt() == 250);

		//String s = SerializeJsonValue(v);
		//printf("%s", s.pData);
		//FreeString(s);

		//FILE* pFile = fopen("tank.gltf", "r");
		//size_t fileSize = 0;
		//String fileContents;
		//defer(FreeString(&gAllocator, fileContents));
		//if (pFile != nullptr) {
		//	fseek(pFile, 0L, SEEK_END);
		//	fileSize = ftell(pFile);
		//	rewind(pFile);
		//	fileContents = AllocString(&gAllocator, fileSize);
		//	fread(fileContents.pData, 1, fileSize, pFile);
		//}
		//
		//JsonValue tankFile = ParseJsonFile(&gAllocator, fileContents);
		//defer(tankFile.Free());
		//bool validGltf = tankFile["asset"]["version"].ToString() == "2.0";
	}
	errorCount += ReportMemoryLeaks();
	EndTest(errorCount);
}

// Goals:
// No copy constructors
// No constructors, POD data everywhere
// Explicit memory operations for things

// SORT
// ------
// We want to implement a quick sort. stl sort switches to heap or merge sort to avoid worst case
// known as introsort, but we don't need this
// When qsort recursion depth goes past some point. How do these algorithms work?
// https://coderslegacy.com/python/quicksort-algorithm/
// https://www.geeksforgeeks.org/quick-sort/


// TODO LIST
// [x] Allocators are abstract interfaces, similar to bx library
// [x] Json parser can use custom allocators
// [ ] Architect functions so that allocators are optional params
// [ ] rename old pointer type in json parser
// [ ] Sort algorithm for ResizableArrays
// [ ] Move math libs into common
// [ ] Move error handling and asserts into common
// [ ] Move utils into common

int main() {
	JsonTest();
	LinearAllocatorTest();
	HashMapTest();
	StringTest();
	ResizableArrayTest();
	__debugbreak();
    return 0;
}