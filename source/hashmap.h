// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

struct String;

// Key functions define all the needed operations
// for each key. So the hashing function and the key comparison
// -------------------------------

template<typename K>
struct KeyFuncs {
    u64 Hash(K key) const {
        return static_cast<u64>(key);
    }

    bool Cmp(K key1, K key2) const {
        return static_cast<u64>(key1) == static_cast<u64>(key2);
    }
};

template<typename T>
struct KeyFuncs<T*> {
    u64 Hash(T* key) const;
    bool Cmp(T* key1, T* key2) const;
};

// Numeric hashes
template<>
struct KeyFuncs<char> {
    u64 Hash(char key) const;
    bool Cmp(char key1, char key2) const;
};

template<>
struct KeyFuncs<i8> {
    u64 Hash(i8 key) const;
    bool Cmp(i8 key1, i8 key2) const;
};

template<>
struct KeyFuncs<int16_t> {
    u64 Hash(int16_t key) const;
    bool Cmp(int16_t key1, int16_t key2) const;
};

template<>
struct KeyFuncs<i32> {
    u64 Hash(i32 key) const;
    bool Cmp(i32 key1, i32 key2) const;
};

template<>
struct KeyFuncs<i64> {
    u64 Hash(i64 key) const;
    bool Cmp(i64 key1, i64 key2) const;
};

template<>
struct KeyFuncs<u8> {
    u64 Hash(u8 key) const;
    bool Cmp(u8 key1, u8 key2) const;
};

template<>
struct KeyFuncs<uint16_t> {
    u64 Hash(uint16_t key) const;
    bool Cmp(uint16_t key1, uint16_t key2) const;
};

template<>
struct KeyFuncs<u32> {
    u64 Hash(u32 key) const;
    bool Cmp(u32 key1, u32 key2) const;
};

template<>
struct KeyFuncs<u64> {
    u64 Hash(u64 key) const;
    bool Cmp(u64 key1, u64 key2) const;
};

template<>
struct KeyFuncs<f32> {
    u64 Hash(f32 key) const;
    bool Cmp(f32 key1, f32 key2) const;
};

template<>
struct KeyFuncs<f64> {
    u64 Hash(f64 key) const;
    bool Cmp(f64 key1, f64 key2) const;
};

template<>
struct KeyFuncs<String> {
    u64 Hash(const String& key) const;
    bool Cmp(const String& key1, const String& key2) const;
};

template<>
struct KeyFuncs<char*> {
    u64 Hash(const char* key) const;
    bool Cmp(const char* key1, const char* key2) const;
};

template<>
struct KeyFuncs<const char*> {
    u64 Hash(const char* key) const;
    bool Cmp(const char* key1, const char* key2) const;
};


#define UNUSED_HASH 0
#define FIRST_VALID_HASH 1

// Hash Node used in hashmap below
// -------------------------------
template<typename K, typename V>
struct HashNode {
    u64 hash { UNUSED_HASH };
    K key;
    V value;
};


// Hashmap data structure
// -----------------------
// Open addressing to reduce memory allocs and improve cache coherency
// TODO Documentation etc

template<typename K, typename V, typename KF = KeyFuncs<K>>
struct HashMap {
    HashNode<K, V>* pTable{nullptr};
    KF keyFuncs;
    i64 tableSize{0};
    i64 count{0};
	Arena* pArena{nullptr};

    HashMap(Arena* pArena);

    V& Add(const K& key, const V& value);

    V* Get(const K& key);

    V& GetOrAdd(const K& key);

    V& operator[](const K& key);

    void Erase(const K& key);

    template<typename F>
    void Erase(const K& key, F&& freeNode);

    void Rehash(i64 requiredTableSize);
};


// implementation

// ***********************************************************************

template<typename T>
inline u64 KeyFuncs<T*>::Hash(T* key) const {
    return u64(uintptr_t(key));
}

// ***********************************************************************

template<typename T>
inline bool KeyFuncs<T*>::Cmp(T* key1, T* key2) const {
    return key1 == key2;
}

// ***********************************************************************

inline u64 KeyFuncs<char>::Hash(char key) const {
    return static_cast<u64>(key);
}

// ***********************************************************************

inline bool KeyFuncs<char>::Cmp(char key1, char key2) const {
    return key1 == key2;
}

// ***********************************************************************

inline u64 KeyFuncs<i8>::Hash(i8 key) const {
    return static_cast<u64>(key);
}

// ***********************************************************************

inline bool KeyFuncs<i8>::Cmp(i8 key1, i8 key2) const {
    return key1 == key2;
}

// ***********************************************************************

inline u64 KeyFuncs<int16_t>::Hash(int16_t key) const {
    return static_cast<u64>(key);
}

// ***********************************************************************

inline bool KeyFuncs<int16_t>::Cmp(int16_t key1, int16_t key2) const {
    return key1 == key2;
}

// ***********************************************************************

inline u64 KeyFuncs<i32>::Hash(i32 key) const {
    return static_cast<u64>(key);
}

// ***********************************************************************

inline bool KeyFuncs<i32>::Cmp(i32 key1, i32 key2) const {
    return key1 == key2;
}

// ***********************************************************************

inline u64 KeyFuncs<i64>::Hash(i64 key) const {
    return static_cast<u64>(key);
}

// ***********************************************************************

inline bool KeyFuncs<i64>::Cmp(i64 key1, i64 key2) const {
    return key1 == key2;
}

// ***********************************************************************

inline u64 KeyFuncs<u8>::Hash(u8 key) const {
    return static_cast<u64>(key);
}

// ***********************************************************************

inline bool KeyFuncs<u8>::Cmp(u8 key1, u8 key2) const {
    return key1 == key2;
}

// ***********************************************************************

inline u64 KeyFuncs<uint16_t>::Hash(uint16_t key) const {
    return static_cast<u64>(key);
}

// ***********************************************************************

inline bool KeyFuncs<uint16_t>::Cmp(uint16_t key1, uint16_t key2) const {
    return key1 == key2;
}

// ***********************************************************************

inline u64 KeyFuncs<u32>::Hash(u32 key) const {
    return static_cast<u64>(key);
}

// ***********************************************************************

inline bool KeyFuncs<u32>::Cmp(u32 key1, u32 key2) const {
    return key1 == key2;
}

// ***********************************************************************

inline u64 KeyFuncs<u64>::Hash(u64 key) const {
    return key;
}

// ***********************************************************************

inline bool KeyFuncs<u64>::Cmp(u64 key1, u64 key2) const {
    return key1 == key2;
}

// ***********************************************************************

inline u64 KeyFuncs<f32>::Hash(f32 key) const {
    return static_cast<u64>(key);
}

// ***********************************************************************

inline bool KeyFuncs<f32>::Cmp(f32 key1, f32 key2) const {
    return key1 == key2;
}

// ***********************************************************************

inline u64 KeyFuncs<f64>::Hash(f64 key) const {
    return static_cast<u64>(key);
}

// ***********************************************************************

inline bool KeyFuncs<f64>::Cmp(f64 key1, f64 key2) const {
    return key1 == key2;
}

// ***********************************************************************

inline u64 KeyFuncs<String>::Hash(const String& key) const {
    u64 nbytes = key.length;
    u64 hash = 0x811C9DC5;
    const u8* pData = (const u8*)key.pData;
    while (nbytes--)
        hash = (*pData++ ^ hash) * 0x01000193;
    return hash;
}

// ***********************************************************************

inline bool KeyFuncs<String>::Cmp(const String& key1, const String& key2) const {
    return key1 == key2;
}

// ***********************************************************************

inline u64 KeyFuncs<char*>::Hash(const char* key) const {
    u32 c;
    u32 hash = 0x811C9DC5;
    while ((c = (u8)*key++) != 0)
        hash = (c ^ hash) * 0x01000193;
    return hash;
}

// ***********************************************************************

inline bool KeyFuncs<char*>::Cmp(const char* key1, const char* key2) const {
    return strcmp(key1, key2) == 0;
}

// ***********************************************************************

inline u64 KeyFuncs<const char*>::Hash(const char* key) const {
    u32 c;
    u32 hash = 0x811C9DC5;
    while ((c = (u8)*key++) != 0)
        hash = (c ^ hash) * 0x01000193;
    return hash;
}

// ***********************************************************************

inline bool KeyFuncs<const char*>::Cmp(const char* key1, const char* key2) const {
    return strcmp(key1, key2) == 0;
}

// ***********************************************************************

template<typename K, typename V, typename KF>
inline HashMap<K, V, KF>::HashMap(Arena* _pArena) {
	pArena = _pArena;
}

// ***********************************************************************

template<typename K, typename V, typename KF>
V& HashMap<K, V, KF>::Add(const K& key, const V& value) {
    f32 loadFactor = tableSize == 0 ? INT_MAX : (f32)(count) / (f32)tableSize;
    if (loadFactor >= 0.9f)
        Rehash(tableSize + 1);

    u64 hash = keyFuncs.Hash(key);
    if (hash < FIRST_VALID_HASH)
        hash += FIRST_VALID_HASH;
    u64 index = hash % tableSize;
    u64 probeCounter = 1;

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

// ***********************************************************************

template<typename K, typename V, typename KF>
V* HashMap<K, V, KF>::Get(const K& key) {
    if (tableSize == 0)
        return nullptr;

    u64 hash = keyFuncs.Hash(key);
    if (hash < FIRST_VALID_HASH)
        hash += FIRST_VALID_HASH;
    u64 index = hash % tableSize;
    u64 probeCounter = 1;

    while (pTable[index].hash != UNUSED_HASH) {
        if (keyFuncs.Cmp(pTable[index].key, key)) {
            return &pTable[index].value;
        }
        index = (index + probeCounter) % tableSize;
        probeCounter++;
    }
    return nullptr;
}

// ***********************************************************************

template<typename K, typename V, typename KF>
V& HashMap<K, V, KF>::GetOrAdd(const K& key) {
    if (tableSize == 0) {
        V value = V();
        return Add(key, value);
    }

    f32 loadFactor = tableSize == 0 ? INT_MAX : (f32)(count) / (f32)tableSize;
    if (loadFactor >= 0.9f)
        Rehash(tableSize + 1);

    u64 hash = keyFuncs.Hash(key);
    if (hash < FIRST_VALID_HASH)
        hash += FIRST_VALID_HASH;
    u64 index = hash % tableSize;
    u64 probeCounter = 1;

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

// ***********************************************************************

template<typename K, typename V, typename KF>
V& HashMap<K, V, KF>::operator[](const K& key) {
    return GetOrAdd(key);
}

// ***********************************************************************

template<typename K, typename V, typename KF>
void HashMap<K, V, KF>::Erase(const K& key) {
    Erase(key, [](HashNode<K, V>) {});
}

// ***********************************************************************

template<typename K, typename V, typename KF>
template<typename F>
void HashMap<K, V, KF>::Erase(const K& key, F&& freeNode) {
    if (tableSize == 0)
        return;

    u64 hash = keyFuncs.Hash(key);
    if (hash < FIRST_VALID_HASH)
        hash += FIRST_VALID_HASH;
    u64 index = hash % tableSize;
    u64 probeCounter = 1;

    while (pTable[index].hash != UNUSED_HASH) {
        if (keyFuncs.Cmp(pTable[index].key, key)) {
            // Found the node
            freeNode(pTable[index]);
            memset(&pTable[index], 0, sizeof(HashNode<K, V>));
            count--;
            return;
        }
        index = (index + probeCounter) % tableSize;
        probeCounter++;
    }
}

// ***********************************************************************

template<typename K, typename V, typename KF>
void HashMap<K, V, KF>::Rehash(i64 requiredTableSize) {
    if (requiredTableSize < tableSize)
        return;

    HashNode<K, V>* pTableOld = pTable;

    // f64 the table size until we can fit required table size
    constexpr u64 minTableSize = 32;
    u64 newTableSize = tableSize == 0 ? minTableSize : tableSize * 2;
    while (newTableSize < (requiredTableSize > minTableSize ? requiredTableSize : minTableSize))
        newTableSize *= 2;

	pTable = (HashNode<K, V>*)ArenaAlloc(pArena, sizeof(HashNode<K, V>)*newTableSize, alignof(HashNode<K, V>), false); 
    memset(pTable, 0, newTableSize * sizeof(HashNode<K, V>));

    u64 oldTableSize = tableSize;
    tableSize = newTableSize;
    for (int i = 0; i < oldTableSize; i++) {
        if (pTableOld[i].hash != UNUSED_HASH) {
            Add(pTableOld[i].key, pTableOld[i].value);
            count--;
        }
    }
}
