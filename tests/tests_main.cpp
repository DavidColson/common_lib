
#include "stack.inl"
#include "defer.h"
#include "hashmap.inl"
#include "json.h"
#include "light_string.h"
#include "log.h"
#include "resizable_array.inl"
#include "sort.h"
#include "string_builder.h"
#include "testing.h"
#include "memory_tracker.h"

#include <stdlib.h>
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

void HashMapTest() {
    StartTest("HashMap Test");
    int errorCount = 0;
    {
		Arena arena{};
        HashMap<int, int> testMap(&arena);

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

        HashMap<String, int> testMap2(&arena);

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
        HashMap<int, int> testMap3(&arena);

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
        HashMap<f32, int> floatMap(&arena);
        floatMap[28.31f] = 1337;
        floatMap[4.1231f] = 1338;
        floatMap[0.78f] = 1339;
        VERIFY(floatMap[28.31f] == 1337);
        VERIFY(floatMap[4.1231f] == 1338);
        VERIFY(floatMap[0.78f] == 1339);

        HashMap<char, int> charMap(&arena);
        charMap['c'] = 1337;
        charMap['8'] = 1338;
        charMap['U'] = 1339;
        VERIFY(charMap['c'] == 1337);
        VERIFY(charMap['8'] == 1338);
        VERIFY(charMap['U'] == 1339);

        HashMap<const char*, int> cStringMap(&arena);
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

        HashMap<int*, int> pointerMap(&arena);
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

        HashMap<CustomKeyType, int> customMap(&arena);
        customMap[one] = 1337;
        customMap[two] = 1338;
        customMap[three] = 1339;
        VERIFY(customMap[one] == 1337);
        VERIFY(customMap[two] == 1338);
        VERIFY(customMap[three] == 1339);

		ArenaFinished(&arena);
    }
    errorCount += ReportMemoryLeaks();
    EndTest(errorCount);
}

void StringTest() {
    StartTest("String Test");
    int errorCount = 0;
    {
		Arena arena{};

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

        String copy = CopyCString("Ducks are cool", &arena);
        VERIFY(copy == "Ducks are cool");
        VERIFY(copy.length == 14);

        String copy2 = CopyString(str, &arena);
        VERIFY(copy2 == "Hi Dave");
        VERIFY(copy2.length == 7);

        String allocated = AllocString(copy.length * sizeof(char), &arena);
        memcpy(allocated.pData, copy.pData, copy.length * sizeof(char));
        VERIFY(allocated == copy);
        VERIFY(allocated != str);
        VERIFY(allocated.length == 14);

        // String Builder, for dynamically constructing strings
        StringBuilder builder(&arena);
        builder.Append("Hello world");
        builder.AppendFormat(" my name is %s", "David");
        builder.AppendChars(" and this is my code bloop", 20);

        String builtString = builder.CreateString(&arena);
        VERIFY(builtString == "Hello world my name is David and this is my code");
        VERIFY(builtString != "Ducks");
        VERIFY(builtString.length == 48);

		ArenaFinished(&arena);
    }

    errorCount += ReportMemoryLeaks();
    EndTest(errorCount);
}

void ResizableArrayTest() {
    StartTest("ResizableArray Test");
    int errorCount = 0;
    {
		Arena arena{};
        ResizableArray<int> testArray(&arena);

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

		ArenaFinished(&arena);
    }
    errorCount += ReportMemoryLeaks();
    EndTest(errorCount);
}

void ArenaTest() {
    StartTest("Arena Test");
    int errorCount = 0;
    {
        Arena davesArena{};

        // No freeing required!
        ResizableArray<int> myArray(&davesArena);
        for (int i = 0; i < 3000; i++) {
            myArray.PushBack(i);
        }

        HashMap<int, int> testMap(&davesArena);
        srand(7);
        for (int i = 100; i > 0; i--) {
            testMap.Add(rand(), i);
        }

        ResizableArray<String> myStringArray(&davesArena);
        for (int i = 0; i < 1000; i++) {
            StringBuilder builder(&davesArena);
            builder.AppendFormat("Hello %i", i);
            myStringArray.PushBack(builder.CreateString(&davesArena, true));
            myStringArray.PushBack(CopyCString("world", &davesArena));
        }

        ArenaFinished(&davesArena);
    }
    errorCount += ReportMemoryLeaks();
    EndTest(errorCount);
}

void JsonTest() {
    StartTest("Json Test");
    int errorCount = 0;
    {
		Arena arena{};
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
        JsonValue v = ParseJsonFile(&arena, jsonString);

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
		ArenaFinished(&arena);
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
		Arena arena{};

        int sorted[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        VERIFY(IsSorted(sorted, 9));

        int unsorted[] = { 4, 8, 9, 2, 1, 3, 7, 6 };
        VERIFY(!IsSorted(unsorted, 8));

        Sort(unsorted, 8);
        VERIFY(IsSorted(unsorted, 8));

        srand(22);
        ResizableArray<int> randomArray(&arena);
        for (int i = 0; i < 1024; i++) {
            randomArray.PushBack(rand());
        }
        VERIFY(!IsSorted(randomArray.pData, randomArray.count));
        Sort(randomArray.pData, randomArray.count);
        VERIFY(IsSorted(randomArray.pData, randomArray.count));

        srand(21);
        ResizableArray<float> randomFloats(&arena);
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
		ArenaFinished(&arena);
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
		Arena arena{};

        Stack<float> stack(&arena);

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

		ArenaFinished(&arena);
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
    // LogTest();
    StackTest();
    ArenaTest();
    ResizableArrayTest();
    StringTest();
    HashMapTest();
    SortTest();
    JsonTest();
    __debugbreak();
    return 0;
}
