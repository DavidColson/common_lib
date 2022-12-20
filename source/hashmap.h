#pragma once

#include "light_string.h"
#include "memory.h"

// Key functions define all the needed operations
// for each key. So the hashing function and the key comparison
// -------------------------------

namespace Internal
{
	template <typename T, T v>
	struct integral_constant
	{
		static constexpr T value = v;
		typedef T value_type;
		typedef integral_constant<T, v> type;

		constexpr operator value_type() const { return value; }
		constexpr value_type operator()() const { return value; }
	};

	template <typename T>
	struct is_enum : public integral_constant<bool, __is_enum(T)> {};

	template<typename T>
	constexpr bool is_enum_v = is_enum<T>::value;

	// utility to disable the generic template specialization that is
	// used for enum types only.
	template <typename T, bool Enabled>
	struct EnableHashIf
	{
	};

	template <typename T>
	struct EnableHashIf<T, true>
	{
		size_t operator()(const T& p) const { return size_t(p); }
	};
}

template<typename K>
struct KeyFuncs : Internal::EnableHashIf<K, Internal::is_enum_v<K>> {
	uint64_t Hash(K key) const {
		return static_cast<uint64_t>(key);
	}

	bool Cmp(K key1, K key2) const {
		return static_cast<uint64_t>(key1) == static_cast<uint64_t>(key2);
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


#define UNUSED_HASH 0
#define FIRST_VALID_HASH 1

// Hash Node used in hashmap below
// -------------------------------
template<typename K, typename V>
struct HashNode {
	uint64_t hash { UNUSED_HASH };
	K key;
	V value;
};


// Hashmap data structure
// -----------------------
// Open addressing to reduce memory allocs and improve cache coherency, requires manual freeing
// TODO Documentation etca

template<typename K, typename V, typename KF = KeyFuncs<K>>
struct HashMap {
	HashNode<K, V>* pTable { nullptr };
	KF keyFuncs;
	size_t tableSize { 0 };
	size_t count { 0 };
	IAllocator* pAlloc { nullptr };

	HashMap(IAllocator* _pAlloc = &gAllocator) {
		pAlloc = _pAlloc;
	}

	void Free() {
		if (pTable)
			pAlloc->Free(pTable);
	}

	template<typename F>
	void Free(F&& freeNode) {
		for (size_t i = 0; i < tableSize; i++) {
			if (pTable[i].hash != UNUSED_HASH) {
				freeNode(pTable[i]);
			}
		}
		pAlloc->Free(pTable);
	}

	V& Add(const K& key, const V& value) {
		float loadFactor = tableSize == 0 ? INT_MAX : (float)(count) / (float)tableSize;
		if (loadFactor >= 0.9f) Rehash(tableSize + 1);

		uint64_t hash = keyFuncs.Hash(key);
		if (hash < FIRST_VALID_HASH) hash += FIRST_VALID_HASH;
		uint64_t index = hash % tableSize;
		uint64_t probeCounter = 1;

		while (pTable[index].hash != UNUSED_HASH) {
			index = (index + probeCounter) % tableSize;
			probeCounter++;
		}

		HashNode<K, V>& node = pTable[index];
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

		float loadFactor = tableSize == 0 ? INT_MAX : (float)(count) / (float)tableSize;
		if (loadFactor >= 0.9f) Rehash(tableSize + 1);

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
		Erase(key, [](HashNode<K, V>) {});
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
				memset(&pTable[index], 0, sizeof(HashNode<K, V>));
				count--;
			}
			index = (index + probeCounter) % tableSize;
			probeCounter++;
		}
	}

	void Rehash(size_t requiredTableSize) {
		if (requiredTableSize < tableSize)
			return;

		HashNode<K, V>* pTableOld = pTable;

		// Double the table size until we can fit required table size
		constexpr size_t minTableSize = 32;
		size_t newTableSize = tableSize == 0 ? minTableSize : tableSize * 2;
		while (newTableSize < (requiredTableSize > minTableSize ? requiredTableSize : minTableSize))
			newTableSize *= 2;

		pTable = (HashNode<K, V>*)pAlloc->Allocate(newTableSize * sizeof(HashNode<K, V>));
		memset(pTable, 0, newTableSize * sizeof(HashNode<K, V>));

		size_t oldTableSize = tableSize;
		tableSize = newTableSize;
		for (int i = 0; i < oldTableSize; i++) {
			if (pTableOld[i].hash != UNUSED_HASH) {
				Add(pTableOld[i].key, pTableOld[i].value);
				count--;
			}
		}
		if (pTableOld) pAlloc->Free(pTableOld);
	}
};