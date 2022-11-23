#pragma once

#include "string.h"
#include "memory.h"

#include <math.h>

#ifdef _DEBUG
#define DEBUG_CHECK(expression) if (!(expression)) __debugbreak();
#else
#define DEBUG_CHECK(expression)
#endif

// Prime number list, used to pick prime number sizes 
// of the hashmap so collisions are less frequent
// -------------------------------------------------

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




// Key functions define all the needed operations
// for each key. So the hashing function and the key comparison
// -------------------------------

template<typename K>
struct KeyFuncs {
	uint64_t Hash(K& key) const
	{
		// This code intentionally doesn't compile
		// You must provide a custom hash for your key type
		DEBUG_CHECK(false)
		return 0;
	}

	bool Cmp(K& key1, K& key2) const {
		return false;
	}
};

template<typename T>
struct KeyFuncs<T*> { 
	uint64_t Hash(T* key) const { return uint64_t(uintptr_t(key)); }
	bool Cmp(T* key1, T* key2) const { return key1 == key2; }
};

// Numeric hashes
template<>
struct KeyFuncs<char> {
	uint64_t Hash(char key) const { return static_cast<uint64_t>(key); }
	bool Cmp(int8_t key1, int8_t key2) const { return key1 == key2; }
};

template<>
struct KeyFuncs<int8_t> {
	uint64_t Hash(int8_t key) const { return static_cast<uint64_t>(key); } 
	bool Cmp(int8_t key1, int8_t key2) const { return key1 == key2; }
};

template<>
struct KeyFuncs<int16_t> {
	uint64_t Hash(int16_t key) const { return static_cast<uint64_t>(key); } 
	bool Cmp(int16_t key1, int16_t key2) const { return key1 == key2; }
};

template<>
struct KeyFuncs<int32_t> {
	uint64_t Hash(int32_t key) const { return static_cast<uint64_t>(key); } 
	bool Cmp(int32_t key1, int32_t key2) const { return key1 == key2; }
};

template<>
struct KeyFuncs<int64_t> {
	uint64_t Hash(int64_t key) const { return static_cast<uint64_t>(key); } 
	bool Cmp(int64_t key1, int64_t key2) const { return key1 == key2; }
};

template<>
struct KeyFuncs<uint8_t> {
	uint64_t Hash(uint8_t key) const { return static_cast<uint64_t>(key); } 
	bool Cmp(uint8_t key1, uint8_t key2) const { return key1 == key2; }
};

template<>
struct KeyFuncs<uint16_t> {
	uint64_t Hash(uint16_t key) const { return static_cast<uint64_t>(key); } 
	bool Cmp(uint16_t key1, uint16_t key2) const { return key1 == key2; }
};

template<>
struct KeyFuncs<uint32_t> {
	uint64_t Hash(uint32_t key) const { return static_cast<uint64_t>(key); } 
	bool Cmp(uint32_t key1, uint32_t key2) const { return key1 == key2; }
};

template<>
struct KeyFuncs<uint64_t> {
	uint64_t Hash(uint64_t key) const { return static_cast<uint64_t>(key); } 
	bool Cmp(uint64_t key1, uint64_t key2) const { return key1 == key2; }
};

template<>
struct KeyFuncs<float> {
	uint64_t Hash(float key) const { return static_cast<uint64_t>(key); } 
	bool Cmp(float key1, float key2) const { return key1 == key2; }
};

template<>
struct KeyFuncs<double> {
	uint64_t Hash(double key) const { return static_cast<uint64_t>(key); } 
	bool Cmp(double key1, double key2) const { return key1 == key2; }
};

template<>
struct KeyFuncs<long double> {
	uint64_t Hash(long double key) const { return static_cast<uint64_t>(key); } 
	bool Cmp(long double key1, long double key2) const { return key1 == key2; }
};

// String hashes
template<>
struct KeyFuncs<String> {
	uint64_t Hash(const String& key) const
	{
		uint32_t nChars = key.length;
		uint32_t hash = 0x811C9DC5;
		const unsigned char* pData = (const unsigned char*)key.pData;
		while (nChars--)
			hash = (*pData++ ^ hash) * 0x01000193;
		return hash;
	}
	bool Cmp(const String& key1, const String& key2) const {
		return strcmp(key1.pData, key2.pData) == 0;
	}
};

template<>
struct KeyFuncs<char*> {
	uint64_t Hash(const char* key) const
	{
		uint32_t c;
		uint32_t hash = 0x811C9DC5;
		while ((c = (uint8_t)*key++) != 0)
			hash = (c ^ hash) * 0x01000193;
		return hash;
	}
	bool Cmp(const char* key1, const char* key2) const {
		return strcmp(key1, key2) == 0;
	}
};

template<>
struct KeyFuncs<const char*> {
	uint64_t Hash(const char* key) const
	{
		uint32_t c;
		uint32_t hash = 0x811C9DC5;
		while ((c = (uint8_t)*key++) != 0)
			hash = (c ^ hash) * 0x01000193;
		return hash;
	}
	bool Cmp(const char* key1, const char* key2) const {
		return strcmp(key1, key2) == 0;
	}
};



// Hash Node used in hashmap below
// -------------------------------
template<typename K, typename V>
struct HashNode {
	K key;
	V value;
	HashNode* pNext{ nullptr };
};



// Hashmap data structure
// -----------------------
// TODO Documentation etc

template<typename K, typename V, typename AllocatorType=Allocator, typename KF = KeyFuncs<K>>
struct HashMap {
	HashNode<K, V>** pTable{ nullptr };
	KF keyFuncs;
	uint32_t size{ 0 };
	uint32_t bucketCount{ 11 };
	AllocatorType allocator;

	HashMap() {
		pTable = (HashNode<K, V>**)allocator.Allocate(bucketCount * sizeof(HashNode<K, V>*));
		for (uint32_t i = 0; i < bucketCount; i++) // Note that placement new array uses the first few bytes for extra stuff, need to avoid this
			pTable[i] = nullptr;
	}

	~HashMap() {
		for (uint32_t i = 0; i < bucketCount; i++) {
			HashNode<K, V>* pEntry = pTable[i];
			while (pEntry != nullptr) {
				HashNode<K, V>* pPrev = pEntry;
				pEntry = pEntry->pNext;
				pPrev->~HashNode<K, V>(); // Note for dave, if you want an example mem leak, delete this line
				allocator.Free(pPrev);
			}
			pTable[i] = nullptr;
		}
		allocator.Free(pTable);
	}


	bool Exists(const K& key) {
		uint32_t hash = keyFuncs.Hash(key) % bucketCount;
		HashNode<K, V>* pEntry = pTable[hash];
		
		while (pEntry != nullptr) {
			if (pEntry->key == key) {
				return true;
			}
			pEntry = pEntry->pNext;
		}
		return false;
	}

	V& operator[](const K& key) {
		uint32_t hash = keyFuncs.Hash(key) % bucketCount;
		HashNode<K, V>* pPrev = nullptr;
		HashNode<K, V>* pEntry = pTable[hash];

		// Find the entry in it's bucket
		while (pEntry != nullptr && !keyFuncs.Cmp(pEntry->key, key)) {
			pPrev = pEntry;
			pEntry = pEntry->pNext;
		}

		// It doesn't exist, make it
		if (pEntry == nullptr) {
			uint32_t newBucketCount;
			if (NeedsRehash(1, newBucketCount)) {
				Rehash(newBucketCount);
				// hash code has changed, so need to find the correct place to put this node
				hash = keyFuncs.Hash(key) % bucketCount;
				if (pTable[hash] != nullptr) {
					pPrev = pTable[hash];
					while (pPrev->pNext != nullptr) {
						pPrev = pPrev->pNext;
					}
				}
				else {
					pPrev = nullptr;
				}
			}

			size++;
			pEntry = (HashNode<K, V>*)allocator.Allocate(sizeof(HashNode<K, V>));
			SYS_P_NEW(pEntry) HashNode<K, V>();
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
		uint32_t hash = keyFuncs.Hash(key) % bucketCount;
		HashNode<K, V>* pPrev = nullptr;
		HashNode<K, V>* pEntry = pTable[hash];

		// Find the entry in it's bucket
		while (pEntry != nullptr && !keyFuncs.Cmp(pEntry->key, key)) {
			pPrev = pEntry;
			pEntry = pEntry->pNext;
		}

		if (pEntry != nullptr) {
			HashNode<K, V>* pNext = pEntry->pNext;
			pEntry->~HashNode<K, V>();
			allocator.Free(pEntry);
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
		HashNode<K, V>** pTableNew = (HashNode<K, V>**)allocator.Allocate(desiredBuckets * sizeof(HashNode<K, V>*));
		for (uint32_t i = 0; i < desiredBuckets; i++) // Note that placement new array uses the first few bytes for extra stuff, need to avoid this
			pTableNew[i] = nullptr;

		for (uint32_t i = 0; i < bucketCount; i++)
		{
			HashNode<K, V>* pEntry = pTable[i];
			while (pEntry != nullptr) {
				// Process pEntry
				uint32_t newHash = keyFuncs.Hash(pEntry->key) % desiredBuckets;

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
};