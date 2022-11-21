
// TODO: put this someplace sensible
#ifdef _DEBUG
#define DEBUG_CHECK(expression) if (!(expression)) __debugbreak();
#else
#define DEBUG_CHECK(expression)
#endif

#include "string_view.h"
#include "string.h"
#include "array.h"
#include "testing.h"

#include <new>

const uint32_t primeNumberArray[] =
{
	2u, 3u, 5u, 7u, 11u, 13u, 17u, 19u, 23u, 29u, 31u,
	37u, 41u, 43u, 47u, 53u, 59u, 61u, 67u, 71u, 73u, 79u,
	83u, 89u, 97u, 103u, 109u, 113u, 127u, 137u, 139u, 149u,
	157u, 167u, 179u, 193u, 199u, 211u, 227u, 241u, 257u,
	277u, 293u, 313u, 337u, 359u, 383u, 409u, 439u, 467u,
	503u, 541u, 577u, 619u, 661u, 709u, 761u, 823u, 887u,
	953u, 1031u, 1109u, 1193u, 1289u, 1381u, 1493u, 1613u,
	1741u, 1879u, 2029u, 2179u, 2357u, 2549u, 2753u, 2971u,
	3209u, 3469u, 3739u, 4027u, 4349u, 4703u, 5087u, 5503u,
	5953u, 6427u, 6949u, 7517u, 8123u, 8783u, 9497u, 10273u,
	11113u, 12011u, 12983u, 14033u, 15173u, 16411u, 17749u,
	19183u, 20753u, 22447u, 24281u, 26267u, 28411u, 30727u,
	33223u, 35933u, 38873u, 42043u, 45481u, 49201u, 53201u,
	57557u, 62233u, 67307u, 72817u, 78779u, 85229u, 92203u,
	99733u, 107897u, 116731u, 126271u, 136607u, 147793u,
	159871u, 172933u, 187091u, 202409u, 218971u, 236897u,
	256279u, 277261u, 299951u, 324503u, 351061u, 379787u,
	410857u, 444487u, 480881u, 520241u, 562841u, 608903u,
	658753u, 712697u, 771049u, 834181u, 902483u, 976369u,
	1056323u, 1142821u, 1236397u, 1337629u, 1447153u, 1565659u,
	1693859u, 1832561u, 1982627u, 2144977u, 2320627u, 2510653u,
	2716249u, 2938679u, 3179303u, 3439651u, 3721303u, 4026031u,
	4355707u, 4712381u, 5098259u, 5515729u, 5967347u, 6456007u,
	6984629u, 7556579u, 8175383u, 8844859u, 9569143u, 10352717u,
	11200489u, 12117689u, 13109983u, 14183539u, 15345007u,
	16601593u, 17961079u, 19431899u, 21023161u, 22744717u,
	24607243u, 26622317u, 28802401u, 31160981u, 33712729u,
	36473443u, 39460231u, 42691603u, 46187573u, 49969847u,
	54061849u, 58488943u, 63278561u, 68460391u, 74066549u,
	80131819u, 86693767u, 93793069u, 101473717u, 109783337u,
	118773397u, 128499677u, 139022417u, 150406843u, 162723577u,
	176048909u, 190465427u, 206062531u, 222936881u, 241193053u,
	260944219u, 282312799u, 305431229u, 330442829u, 357502601u,
	386778277u, 418451333u, 452718089u, 489790921u, 529899637u,
	573292817u, 620239453u, 671030513u, 725980837u, 785430967u,
	849749479u, 919334987u, 994618837u, 1076067617u, 1164186217u,
	1259520799u, 1362662261u, 1474249943u, 1594975441u,
	1725587117u, 1866894511u, 2019773507u, 2185171673u,
	2364114217u, 2557710269u, 2767159799u, 2993761039u,
	3238918481u, 3504151727u, 3791104843u, 4101556399u,
	4294967291u
};
const uint32_t primeCount = (sizeof(primeNumberArray) / sizeof(primeNumberArray[0]) - 1);

template<typename K>
struct Hash {
	uint64_t operator()(const K& key) const
	{
		// TODO: Compile error here?
		DEBUG_CHECK(false) // Must provide a custom hash for your key type
		return 0;
	}
};

template<>
struct Hash<String> {
	uint64_t operator()(const String& key) const
	{
		uint32_t nChars = key.length;
		uint32_t hash = 0x811C9DC5;
		const unsigned char* pData = (const unsigned char*)key.pData;
		while (nChars--)
			hash = (*pData++ ^ hash) * 0x01000193;
		return hash;
	}
};

template<typename K, typename V>
struct HashNode {
	K key;
	V value;
	HashNode* pNext{ nullptr };
};

template<typename K, typename V, typename H = Hash<K>>
struct HashMap {
	HashMap() {
		pTable = (HashNode<K, V>**)malloc(bucketCount * sizeof(HashNode<K, V>*));
		for (uint32_t i = 0; i < bucketCount; i++) // Note that placement new array uses the first few bytes for extra stuff, need to avoid this
			pTable[i] = nullptr;
	}

	~HashMap() {
		// Dealloc all the nodes
		// Dealloc the table
	}

	bool Check(const K& key, V& outValue) {
		uint32_t hash = hashFunc(key) % bucketCount;
		HashNode<K, V>* pEntry = pTable[hash];
		
		while (pEntry != nullptr) {
			if (pEntry->key == key) {
				outValue = pEntry->value;
				return true;
			}
			pEntry = pEntry->pNext;
		}
		return false;
	}

	V& operator[](const K& key) {
		uint32_t hash = hashFunc(key) % bucketCount;
		HashNode<K, V>* pPrev = nullptr;
		HashNode<K, V>* pEntry = pTable[hash];

		// Find the entry in it's bucket
		while (pEntry != nullptr && pEntry->key != key) {
			pPrev = pEntry;
			pEntry = pEntry->pNext;
		}

		// It doesn't exist, make it
		if (pEntry == nullptr) {
			uint32_t newBucketCount;
			if (NeedsRehash(1, newBucketCount))
				Rehash(newBucketCount);

			size++;
			pEntry = (HashNode<K, V>*)malloc(sizeof(HashNode<K, V>));
			new (pEntry) HashNode<K, V>();
			pEntry->key = key;
			pEntry->pNext = nullptr;

			if (pPrev == nullptr) {
				pTable[hash] = pEntry;
			}
			else {
				pPrev->pNext = pEntry;
			}
		}
		return pEntry->value;
	}

	void Erase(const K& key) {
		uint32_t hash = hashFunc(key) % bucketCount;
		HashNode<K, V>* pPrev = nullptr;
		HashNode<K, V>* pEntry = pTable[hash];

		// Find the entry in it's bucket
		while (pEntry != nullptr && pEntry->key != key) {
			pPrev = pEntry;
			pEntry = pEntry->pNext;
		}

		if (pEntry != nullptr) {
			HashNode<K, V>* pNext = pEntry->pNext;
			delete pEntry;
			size--;

			if (pPrev == nullptr) {
				if (pNext != nullptr)
					pTable[hash] = pNext;
				else
					pTable[hash] = nullptr;
			}
			else {
				pPrev->pNext = pNext;
			}
		}
	}

	void Rehash(uint32_t desiredBuckets) {
		HashNode<K, V>** pTableNew = (HashNode<K, V>**)malloc(desiredBuckets * sizeof(HashNode<K, V>*));
		for (uint32_t i = 0; i < desiredBuckets; i++) // Note that placement new array uses the first few bytes for extra stuff, need to avoid this
			pTableNew[i] = nullptr;

		for (uint32_t i = 0; i < bucketCount; i++)
		{
			HashNode<K, V>* pEntry = pTable[i];
			while (pEntry != nullptr) {
				// Process pEntry
				uint32_t newHash = hashFunc(pEntry->key) % desiredBuckets;

				if (pTableNew[newHash] != nullptr) {
					HashNode<K, V>* pPrev = pTableNew[newHash];
					HashNode<K, V>* pNext = pPrev->pNext;
					while (pNext != nullptr) {
						pPrev = pNext;
						pNext = pPrev->pNext;
					}
					pPrev->pNext = pEntry;
				}
				else {
					pTableNew[newHash] = pEntry;
				}
				HashNode<K, V>* pTemp = pEntry;
				pEntry = pEntry->pNext;
				pTemp->pNext = nullptr;
			}
		}

		free(pTable);
		pTable = pTableNew;
		bucketCount = desiredBuckets;
	}

	uint32_t GetIdealBucketCount(uint32_t elements, float maxLoadFactor = 1.0f) {
		float minPossibleBucketCount = elements / maxLoadFactor;
		uint32_t smallestPrime = 0;
		for (uint32_t i = 1; i < primeCount; i++) {
			if (primeNumberArray[i] > minPossibleBucketCount)
			{
				smallestPrime = primeNumberArray[i];
				break;
			}
		}
		return (uint32_t)ceil(smallestPrime * maxLoadFactor);
	}

	bool NeedsRehash(uint32_t elemsToAdd, uint32_t& outNewBucketCount) {
		// Settings
		constexpr float growthFactor = 2.0f;
		constexpr float maxLoadFactor = 1.0f;

		float minPossibleBucketCount = (size + elemsToAdd) / maxLoadFactor;
		if (minPossibleBucketCount > (float)bucketCount) {
			// Needs a resize
			minPossibleBucketCount = (float)fmax(growthFactor * bucketCount, minPossibleBucketCount);
			outNewBucketCount = GetIdealBucketCount(size + elemsToAdd, maxLoadFactor);
			return true;
		}
		return false;
	}

	float LoadFactor() const {
		return (float)size / (float)bucketCount;
	}

	HashNode<K, V>** pTable{ nullptr };
	H hashFunc;
	uint32_t size{ 0 };
	uint32_t bucketCount{ 11 };
};

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