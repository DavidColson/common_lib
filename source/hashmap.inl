#include "hashmap.h"

#include "light_string.h"

#include <string.h>

// ***********************************************************************

template<typename T>
inline uint64_t KeyFuncs<T*>::Hash(T* key) const {
    return uint64_t(uintptr_t(key));
}

// ***********************************************************************

template<typename T>
inline bool KeyFuncs<T*>::Cmp(T* key1, T* key2) const {
    return key1 == key2;
}

// ***********************************************************************

inline uint64_t KeyFuncs<char>::Hash(char key) const {
    return static_cast<uint64_t>(key);
}

// ***********************************************************************

inline bool KeyFuncs<char>::Cmp(char key1, char key2) const {
    return key1 == key2;
}

// ***********************************************************************

inline uint64_t KeyFuncs<int8_t>::Hash(int8_t key) const {
    return static_cast<uint64_t>(key);
}

// ***********************************************************************

inline bool KeyFuncs<int8_t>::Cmp(int8_t key1, int8_t key2) const {
    return key1 == key2;
}

// ***********************************************************************

inline uint64_t KeyFuncs<int16_t>::Hash(int16_t key) const {
    return static_cast<uint64_t>(key);
}

// ***********************************************************************

inline bool KeyFuncs<int16_t>::Cmp(int16_t key1, int16_t key2) const {
    return key1 == key2;
}

// ***********************************************************************

inline uint64_t KeyFuncs<int32_t>::Hash(int32_t key) const {
    return static_cast<uint64_t>(key);
}

// ***********************************************************************

inline bool KeyFuncs<int32_t>::Cmp(int32_t key1, int32_t key2) const {
    return key1 == key2;
}

// ***********************************************************************

inline uint64_t KeyFuncs<int64_t>::Hash(int64_t key) const {
    return static_cast<uint64_t>(key);
}

// ***********************************************************************

inline bool KeyFuncs<int64_t>::Cmp(int64_t key1, int64_t key2) const {
    return key1 == key2;
}

// ***********************************************************************

inline uint64_t KeyFuncs<uint8_t>::Hash(uint8_t key) const {
    return static_cast<uint64_t>(key);
}

// ***********************************************************************

inline bool KeyFuncs<uint8_t>::Cmp(uint8_t key1, uint8_t key2) const {
    return key1 == key2;
}

// ***********************************************************************

inline uint64_t KeyFuncs<uint16_t>::Hash(uint16_t key) const {
    return static_cast<uint64_t>(key);
}

// ***********************************************************************

inline bool KeyFuncs<uint16_t>::Cmp(uint16_t key1, uint16_t key2) const {
    return key1 == key2;
}

// ***********************************************************************

inline uint64_t KeyFuncs<uint32_t>::Hash(uint32_t key) const {
    return static_cast<uint64_t>(key);
}

// ***********************************************************************

inline bool KeyFuncs<uint32_t>::Cmp(uint32_t key1, uint32_t key2) const {
    return key1 == key2;
}

// ***********************************************************************

inline uint64_t KeyFuncs<uint64_t>::Hash(uint64_t key) const {
    return key;
}

// ***********************************************************************

inline bool KeyFuncs<uint64_t>::Cmp(uint64_t key1, uint64_t key2) const {
    return key1 == key2;
}

// ***********************************************************************

inline uint64_t KeyFuncs<float>::Hash(float key) const {
    return static_cast<uint64_t>(key);
}

// ***********************************************************************

inline bool KeyFuncs<float>::Cmp(float key1, float key2) const {
    return key1 == key2;
}

// ***********************************************************************

inline uint64_t KeyFuncs<double>::Hash(double key) const {
    return static_cast<uint64_t>(key);
}

// ***********************************************************************

inline bool KeyFuncs<double>::Cmp(double key1, double key2) const {
    return key1 == key2;
}

// ***********************************************************************

inline uint64_t KeyFuncs<long double>::Hash(long double key) const {
    return static_cast<uint64_t>(key);
}

// ***********************************************************************

inline bool KeyFuncs<long double>::Cmp(long double key1, long double key2) const {
    return key1 == key2;
}

// ***********************************************************************

inline uint64_t KeyFuncs<String>::Hash(const String& key) const {
    size_t nChars = key.m_length;
    size_t hash = 0x811C9DC5;
    const unsigned char* pData = (const unsigned char*)key.m_pData;
    while (nChars--)
        hash = (*pData++ ^ hash) * 0x01000193;
    return hash;
}

// ***********************************************************************

inline bool KeyFuncs<String>::Cmp(const String& key1, const String& key2) const {
    return key1 == key2;
}

// ***********************************************************************

inline uint64_t KeyFuncs<char*>::Hash(const char* key) const {
    uint32_t c;
    uint32_t hash = 0x811C9DC5;
    while ((c = (uint8_t)*key++) != 0)
        hash = (c ^ hash) * 0x01000193;
    return hash;
}

// ***********************************************************************

inline bool KeyFuncs<char*>::Cmp(const char* key1, const char* key2) const {
    return strcmp(key1, key2) == 0;
}

// ***********************************************************************

inline uint64_t KeyFuncs<const char*>::Hash(const char* key) const {
    uint32_t c;
    uint32_t hash = 0x811C9DC5;
    while ((c = (uint8_t)*key++) != 0)
        hash = (c ^ hash) * 0x01000193;
    return hash;
}

// ***********************************************************************

inline bool KeyFuncs<const char*>::Cmp(const char* key1, const char* key2) const {
    return strcmp(key1, key2) == 0;
}

// ***********************************************************************

template<typename K, typename V, typename KF>
inline HashMap<K, V, KF>::HashMap(IAllocator* _pAlloc) {
    m_pAlloc = _pAlloc;
}

// ***********************************************************************

template<typename K, typename V, typename KF>
inline void HashMap<K, V, KF>::Free() {
    if (m_pTable)
        m_pAlloc->Free(m_pTable);
}

// ***********************************************************************

template<typename K, typename V, typename KF>
template<typename F> 
inline void HashMap<K, V, KF>::Free(F&& freeNode) {
    for (size_t i = 0; i < m_tableSize; i++) {
        if (m_pTable[i].hash != UNUSED_HASH) {
            freeNode(m_pTable[i]);
        }
    }
    m_pAlloc->Free(m_pTable);
}

// ***********************************************************************

template<typename K, typename V, typename KF>
V& HashMap<K, V, KF>::Add(const K& key, const V& value) {
    float loadFactor = m_tableSize == 0 ? INT_MAX : (float)(m_count) / (float)m_tableSize;
    if (loadFactor >= 0.9f)
        Rehash(m_tableSize + 1);

    uint64_t hash = m_keyFuncs.Hash(key);
    if (hash < FIRST_VALID_HASH)
        hash += FIRST_VALID_HASH;
    uint64_t index = hash % m_tableSize;
    uint64_t probeCounter = 1;

    while (m_pTable[index].hash != UNUSED_HASH) {
        index = (index + probeCounter) % m_tableSize;
        probeCounter++;
    }

    HashNode<K, V>& node = m_pTable[index];
    node.hash = hash;
    node.key = key;
    node.value = value;
    m_count++;
    return node.value;
}

// ***********************************************************************

template<typename K, typename V, typename KF>
V* HashMap<K, V, KF>::Get(const K& key) {
    if (m_tableSize == 0)
        return nullptr;

    uint64_t hash = m_keyFuncs.Hash(key);
    if (hash < FIRST_VALID_HASH)
        hash += FIRST_VALID_HASH;
    uint64_t index = hash % m_tableSize;
    uint64_t probeCounter = 1;

    while (m_pTable[index].hash != UNUSED_HASH) {
        if (m_keyFuncs.Cmp(m_pTable[index].key, key)) {
            return &m_pTable[index].value;
        }
        index = (index + probeCounter) % m_tableSize;
        probeCounter++;
    }
    return nullptr;
}

// ***********************************************************************

template<typename K, typename V, typename KF>
V& HashMap<K, V, KF>::GetOrAdd(const K& key) {
    if (m_tableSize == 0) {
        V value = V();
        return Add(key, value);
    }

    float loadFactor = m_tableSize == 0 ? INT_MAX : (float)(m_count) / (float)m_tableSize;
    if (loadFactor >= 0.9f)
        Rehash(m_tableSize + 1);

    uint64_t hash = m_keyFuncs.Hash(key);
    if (hash < FIRST_VALID_HASH)
        hash += FIRST_VALID_HASH;
    uint64_t index = hash % m_tableSize;
    uint64_t probeCounter = 1;

    while (m_pTable[index].hash != UNUSED_HASH) {
        if (m_keyFuncs.Cmp(m_pTable[index].key, key)) {
            return m_pTable[index].value;
        }
        index = (index + probeCounter) % m_tableSize;
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
    if (m_tableSize == 0)
        return;

    uint64_t hash = m_keyFuncs.Hash(key);
    if (hash < FIRST_VALID_HASH)
        hash += FIRST_VALID_HASH;
    uint64_t index = hash % m_tableSize;
    uint64_t probeCounter = 1;

    while (m_pTable[index].hash != UNUSED_HASH) {
        if (m_keyFuncs.Cmp(m_pTable[index].key, key)) {
            // Found the node
            freeNode(m_pTable[index]);
            memset(&m_pTable[index], 0, sizeof(HashNode<K, V>));
            m_count--;
        }
        index = (index + probeCounter) % m_tableSize;
        probeCounter++;
    }
}

// ***********************************************************************

template<typename K, typename V, typename KF>
void HashMap<K, V, KF>::Rehash(size_t requiredTableSize) {
    if (requiredTableSize < m_tableSize)
        return;

    HashNode<K, V>* pTableOld = m_pTable;

    // Double the table size until we can fit required table size
    constexpr size_t minTableSize = 32;
    size_t newTableSize = m_tableSize == 0 ? minTableSize : m_tableSize * 2;
    while (newTableSize < (requiredTableSize > minTableSize ? requiredTableSize : minTableSize))
        newTableSize *= 2;

    m_pTable = (HashNode<K, V>*)m_pAlloc->Allocate(newTableSize * sizeof(HashNode<K, V>));
    memset(m_pTable, 0, newTableSize * sizeof(HashNode<K, V>));

    size_t oldTableSize = m_tableSize;
    m_tableSize = newTableSize;
    for (int i = 0; i < oldTableSize; i++) {
        if (pTableOld[i].hash != UNUSED_HASH) {
            Add(pTableOld[i].key, pTableOld[i].value);
            m_count--;
        }
    }
    if (pTableOld)
        m_pAlloc->Free(pTableOld);
}