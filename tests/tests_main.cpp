
#pragma warning (disable : 5105)
#include "Windows.h"
#include "dbghelp.h"
#undef min
#undef max
#pragma comment(lib, "gdi32")
#pragma comment(lib, "kernel32")
#pragma comment(lib, "psapi")
#pragma comment(lib, "dbghelp")

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdint.h>

#include "common_lib.h"
#include "common_lib.cpp"

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
    u64 Hash(const CustomKeyType& key) const {
        // This is probably not a good hash method for this type
        // But it serves for demonstration
        return static_cast<u64>(key.thing + key.thing2);
    }
    bool Cmp(const CustomKeyType& key1, const CustomKeyType& key2) const {
        return key1.thing == key2.thing && key1.thing2 == key2.thing2;
    }
};

enum MyTestEnum {
	VAL1,
	VAL2,
	VAL3
};

void HashMapTest() {
    StartTest("HashMap Test");
    int errorCount = 0;
    {
		Arena* pArena = ArenaCreate();
        HashMap<int, int> testMap(pArena);

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

        HashMap<String, int> testMap2(pArena);

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
        HashMap<int, int> testMap3(pArena);

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
        HashMap<f32, int> floatMap(pArena);
        floatMap[28.31f] = 1337;
        floatMap[4.1231f] = 1338;
        floatMap[0.78f] = 1339;
        VERIFY(floatMap[28.31f] == 1337);
        VERIFY(floatMap[4.1231f] == 1338);
        VERIFY(floatMap[0.78f] == 1339);

        HashMap<char, int> charMap(pArena);
        charMap['c'] = 1337;
        charMap['8'] = 1338;
        charMap['U'] = 1339;
        VERIFY(charMap['c'] == 1337);
        VERIFY(charMap['8'] == 1338);
        VERIFY(charMap['U'] == 1339);

		HashMap<MyTestEnum, int> enumMap(pArena);
        enumMap[VAL2] = 1337;
        enumMap[VAL1] = 1338;
        enumMap[VAL3] = 1339;
        VERIFY(enumMap[VAL2] == 1337);
        VERIFY(enumMap[VAL1] == 1338);
        VERIFY(enumMap[VAL3] == 1339);

        HashMap<const char*, int> cStringMap(pArena);
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

        HashMap<int*, int> pointerMap(pArena);
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

        HashMap<CustomKeyType, int> customMap(pArena);
        customMap[one] = 1337;
        customMap[two] = 1338;
        customMap[three] = 1339;
        VERIFY(customMap[one] == 1337);
        VERIFY(customMap[two] == 1338);
        VERIFY(customMap[three] == 1339);

		ArenaFinished(pArena);
    }
    errorCount += ReportMemoryLeaks();
    EndTest(errorCount);
}

void StringTest() {
    StartTest("String Test");
    int errorCount = 0;
    {
		Arena* pArena = ArenaCreate();

        String emptyStr;
        VERIFY(emptyStr.pData == nullptr);
        VERIFY(emptyStr.length == 0);

        String str("Hello World");
        VERIFY(str == "Hello World");
        VERIFY(str.length == 11);

		String str2("heLLo wOrld"); 
		VERIFY(StrCmp(str, str2) == false);
		VERIFY(StrCmp(str, str2, CaseInsensitive) == true);

        str = "Hi Dave";
        VERIFY(str == "Hi Dave");
        VERIFY(str != "Hello World");
        VERIFY(str.length == 7);

		VERIFY(StartsWith(str, "Hi"));
		VERIFY(EndsWith(str, "ave"));

		VERIFY(Prefix(str, 2) == "Hi");
		VERIFY(Postfix(str, 4) == "Dave");
		VERIFY(ChopLeft(str, 3) == "Dave");
		VERIFY(ChopRight(str, 3) == "Hi D");

		String path("C:/drive/applications/polybox/demo/model.gltf");
		ResizableArray<String> tokens = Split(pArena, path, "/.");
		VERIFY(tokens[0] == "C:");
		VERIFY(tokens[1] == "drive");
		VERIFY(tokens[2] == "applications");
		VERIFY(tokens[3] == "polybox");
		VERIFY(tokens[4] == "demo");
		VERIFY(tokens[5] == "model");
		VERIFY(tokens[6] == "gltf");

		VERIFY(TakeAfterLastSlash(path) == "model.gltf");
		VERIFY(TakeBeforeLastSlash(path) == "C:/drive/applications/polybox/demo");
		VERIFY(TakeAfterLastDot(path) == "gltf");
		VERIFY(TakeBeforeLastDot(path) == "C:/drive/applications/polybox/demo/model");

		VERIFY(Join(pArena, tokens, "|") == "C:|drive|applications|polybox|demo|model|gltf");

        String copy = CopyCString("Ducks are cool", pArena);
        VERIFY(copy == "Ducks are cool");
        VERIFY(copy.length == 14);

        String copy2 = CopyString(str, pArena);
        VERIFY(copy2 == "Hi Dave");
        VERIFY(copy2.length == 7);

        String allocated = AllocString(copy.length * sizeof(char), pArena);
        memcpy(allocated.pData, copy.pData, copy.length * sizeof(char));
        VERIFY(allocated == copy);
        VERIFY(allocated != str);
        VERIFY(allocated.length == 14);

        // String Builder, for dynamically constructing strings
        StringBuilder builder(pArena);
        builder.Append("Hello world");
        builder.AppendFormat(" my name is %s", "David");
        builder.AppendChars(" and this is my code bloop", 20);

        String builtString = builder.CreateString(pArena);
        VERIFY(builtString == "Hello world my name is David and this is my code");
        VERIFY(builtString != "Ducks");
        VERIFY(builtString.length == 48);

		ArenaFinished(pArena);
    }

    errorCount += ReportMemoryLeaks();
    EndTest(errorCount);
}

void ResizableArrayTest() {
    StartTest("ResizableArray Test");
    int errorCount = 0;
    {
		Arena* pArena = ArenaCreate();
        ResizableArray<int> testArray(pArena);

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
        // Remove element at back another way (this caused an infinite loop once so we now test for it
        testArray.PushBack(7);
        testArray.Erase(testArray.count - 1);
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

        // Resize an array (does not change capacity if lower, (memory can be reused)
        testArray.Resize(3);
        VERIFY(testArray.count == 3);
        VERIFY(testArray.capacity == 50);

		ArenaFinished(pArena);
    }
    errorCount += ReportMemoryLeaks();
    EndTest(errorCount);
}

void ArenaTest() {
    StartTest("Arena Test");
    int errorCount = 0;
    {
        Arena* pArena = ArenaCreate();

        // No freeing required!
        ResizableArray<int> myArray(pArena);
        for (int i = 0; i < 3000; i++) {
            myArray.PushBack(i);
        }

        HashMap<int, int> testMap(pArena);
        srand(7);
        for (int i = 100; i > 0; i--) {
            testMap.Add(rand(), i);
        }

        ResizableArray<String> myStringArray(pArena);
        for (int i = 0; i < 1000; i++) {
            StringBuilder builder(pArena);
            builder.AppendFormat("Hello %i", i);
            myStringArray.PushBack(builder.CreateString(pArena, true));
            myStringArray.PushBack(CopyCString("world", pArena));
        }

        ArenaFinished(pArena);
    }
    errorCount += ReportMemoryLeaks();
    EndTest(errorCount);
}

void JsonTest() {
    StartTest("Json Test");
    int errorCount = 0;
    {
		Arena* pArena = ArenaCreate();
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
        JsonValue v = ParseJsonFile(pArena, jsonString);

        VERIFY(v["widget"]["debug"].ToString() == "on");
        VERIFY(v["widget"]["window"]["width"].ToInt() == 500);
        VERIFY(v["widget"]["image"]["src"].ToString() == "Images/Sun.png");
        VERIFY(v["widget"]["text"]["hOffset"].ToInt() == 250);

        // String s = SerializeJsonValue(v);
        // printf("%s", s.pData);
        // FreeString(s);

        // FILE* pFile = fopen("tank.gltf", "r");
        // usize fileSize = 0;
        // String fileContents;
        // defer(FreeString(&gAllocator, fileContents));
        // if (pFile != nullptr) {
        //	fseek(pFile, 0L, SEEK_END);
        //	fileSize = ftell(pFile);
        //	rewind(pFile);
        //	fileContents = AllocString(&gAllocator, fileSize);
        //	fread(fileContents.pData, 1, fileSize, pFile);
        // }
        //
        // JsonValue tankFile = ParseJsonFile(&gAllocator, fileContents);
        // defer(tankFile.Free());
        // bool validGltf = tankFile["asset"]["version"].ToString() == "2.0";
		ArenaFinished(pArena);
    }
    errorCount += ReportMemoryLeaks();
    EndTest(errorCount);
}

struct CustomSortableType {
    int height;
    int width;
};

struct SortByHeight {
    bool operator()(const CustomSortableType& a, const CustomSortableType& b) {
        return a.height > b.height;
    }
};

void SortTest() {
    StartTest("Sort");
    int errorCount = 0;

    {
		Arena* pArena = ArenaCreate();

        int sorted[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        VERIFY(IsSorted(sorted, 9));

        int unsorted[] = { 4, 8, 9, 2, 1, 3, 7, 6 };
        VERIFY(!IsSorted(unsorted, 8));

        Sort(unsorted, 8);
        VERIFY(IsSorted(unsorted, 8));

        srand(22);
        ResizableArray<int> randomArray(pArena);
        for (int i = 0; i < 1024; i++) {
            randomArray.PushBack(rand());
        }
        VERIFY(!IsSorted(randomArray.pData, randomArray.count));
        Sort(randomArray.pData, randomArray.count);
        VERIFY(IsSorted(randomArray.pData, randomArray.count));

        srand(21);
        ResizableArray<float> randomFloats(pArena);
        for (int i = 0; i < 1024; i++) {
            float x = (float)rand() / (float)(RAND_MAX / 100.0f);
            randomFloats.PushBack(x);
        }
        VERIFY(!IsSorted(randomFloats.pData, randomFloats.count));
        Sort(randomFloats.pData, randomFloats.count);
        VERIFY(IsSorted(randomFloats.pData, randomFloats.count));

        // Custom ordering
        int descending[] = { 4, 8, 9, 2, 1, 3, 7, 6 };
        Sort(descending, 8, SortDescending<int>());
        VERIFY(IsSorted(descending, 8, SortDescending<int>()));

        // Custom types
        CustomSortableType custom[] = {
            CustomSortableType { 12, 20 },
            CustomSortableType { 4, 10 },
            CustomSortableType { 6, 8 },
            CustomSortableType { 24, 17 },
            CustomSortableType { 2, 14 },
        };

        Sort(custom, 5, SortByHeight());
        VERIFY(IsSorted(custom, 5, SortByHeight()));
		ArenaFinished(pArena);
    }
    errorCount += ReportMemoryLeaks();
    EndTest(errorCount);
}

void CustomLogHandler(Log::LogLevel level, String message) {
    printf("Custom Log Handler Test! %s", message.pData);
}

void LogTest() {
    // Not quite sure how to test this but this is an example usage anyway
    StartTest("Log");
    int errorCount = 0;

    {
        // Basic Logging;
        Log::Debug("Debug log %i", 5);
        Log::Info("Info message %i", 5);
        Log::Warn("Warning %i", 5);

        // Modifying Log Level
        Log::SetLogLevel(Log::EWarn);
        Log::Debug("Debug log %i", 5);
        Log::Info("Info message %i", 5);
        Log::Warn("Warning %i", 5);

        // Criticals can cause a crash and show a callstack (uncomment to see)
        // Log::Crit("Critical Error %i", 5);

        // Assertions
        Assert(5 == 5);
        // Assert(5 == 4);
        // AssertMsg(5 == 4, "woops");

        // Custom handlers
        Log::SetLogLevel(Log::EDebug);
        Log::LogConfig cfg;
        cfg.customHandler1 = CustomLogHandler;
        Log::SetConfig(cfg);

        Log::Debug("A Debug Message");
    }

    errorCount += ReportMemoryLeaks();
    EndTest(errorCount);
}

void StackTest() {
    StartTest("Stack");
    int errorCount = 0;
    {
		Arena* pArena = ArenaCreate();

        Stack<float> stack(pArena);

        stack.Push(7);
        stack.Push(8);
        stack.Push(9);

        VERIFY(stack.Pop() == 9);
        VERIFY(stack.Pop() == 8);
        VERIFY(stack.Pop() == 7);

        stack.Push(12);
        stack.Push(17);
        stack.Push(19);

        VERIFY(stack.Top() == 19);
        VERIFY(stack.Top() == 19);

        // Normal indices
        VERIFY(stack[0] == 12);
        VERIFY(stack[1] == 17);
        VERIFY(stack[2] == 19);

        // Negative indices
        VERIFY(stack[-3] == 12);
        VERIFY(stack[-2] == 17);
        VERIFY(stack[-1] == 19);

		ArenaFinished(pArena);
    }
    errorCount += ReportMemoryLeaks();
    EndTest(errorCount);
}

// Goals:
// No copy constructors
// No constructors, POD data everywhere
// Explicit memory operations for things

// TODO LIST
// [ ] tests for maths code (vec, matrix, quat, AABB)
// [ ] tests for base64
// [ ] tests for scanner

int main() {
	g_pArenaFrame = ArenaCreate();
	g_pArenaPermenant = ArenaCreate();

    // LogTest();
    StackTest();
    ArenaTest();
    ResizableArrayTest();
    StringTest();
    HashMapTest();
    SortTest();
    JsonTest();
    // __debugbreak();
    return 0;
}
