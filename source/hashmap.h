// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

#include "memory.h"

#include <stdint.h>

struct String;

// Key functions define all the needed operations
// for each key. So the hashing function and the key comparison
// -------------------------------

namespace Internal {
template<typename T, T v>
struct integral_constant {
    static constexpr T value = v;
    typedef T value_type;
    typedef integral_constant<T, v> type;

    constexpr operator value_type() const {
        return value;
    }
    constexpr value_type operator()() const {
        return value;
    }
};

template<typename T>
struct is_enum : public integral_constant<bool, __is_enum(T)> {};

template<typename T>
constexpr bool is_enum_v = is_enum<T>::value;

// utility to disable the generic template specialization that is
// used for enum types only.
template<typename T, bool Enabled>
struct EnableHashIf {};

template<typename T>
struct EnableHashIf<T, true> {
    size_t operator()(const T& p) const {
        return size_t(p);
    }
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
    uint64_t Hash(T* key) const;
    bool Cmp(T* key1, T* key2) const;
};

// Numeric hashes
template<>
struct KeyFuncs<char> {
    uint64_t Hash(char key) const;
    bool Cmp(char key1, char key2) const;
};

template<>
struct KeyFuncs<int8_t> {
    uint64_t Hash(int8_t key) const;
    bool Cmp(int8_t key1, int8_t key2) const;
};

template<>
struct KeyFuncs<int16_t> {
    uint64_t Hash(int16_t key) const;
    bool Cmp(int16_t key1, int16_t key2) const;
};

template<>
struct KeyFuncs<int32_t> {
    uint64_t Hash(int32_t key) const;
    bool Cmp(int32_t key1, int32_t key2) const;
};

template<>
struct KeyFuncs<int64_t> {
    uint64_t Hash(int64_t key) const;
    bool Cmp(int64_t key1, int64_t key2) const;
};

template<>
struct KeyFuncs<uint8_t> {
    uint64_t Hash(uint8_t key) const;
    bool Cmp(uint8_t key1, uint8_t key2) const;
};

template<>
struct KeyFuncs<uint16_t> {
    uint64_t Hash(uint16_t key) const;
    bool Cmp(uint16_t key1, uint16_t key2) const;
};

template<>
struct KeyFuncs<uint32_t> {
    uint64_t Hash(uint32_t key) const;
    bool Cmp(uint32_t key1, uint32_t key2) const;
};

template<>
struct KeyFuncs<uint64_t> {
    uint64_t Hash(uint64_t key) const;
    bool Cmp(uint64_t key1, uint64_t key2) const;
};

template<>
struct KeyFuncs<float> {
    uint64_t Hash(float key) const;
    bool Cmp(float key1, float key2) const;
};

template<>
struct KeyFuncs<double> {
    uint64_t Hash(double key) const;
    bool Cmp(double key1, double key2) const;
};

template<>
struct KeyFuncs<long double> {
    uint64_t Hash(long double key) const;
    bool Cmp(long double key1, long double key2) const;
};

template<>
struct KeyFuncs<String> {
    uint64_t Hash(const String& key) const;
    bool Cmp(const String& key1, const String& key2) const;
};

template<>
struct KeyFuncs<char*> {
    uint64_t Hash(const char* key) const;
    bool Cmp(const char* key1, const char* key2) const;
};

template<>
struct KeyFuncs<const char*> {
    uint64_t Hash(const char* key) const;
    bool Cmp(const char* key1, const char* key2) const;
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
    HashNode<K, V>* m_pTable { nullptr };
    KF m_keyFuncs;
    size_t m_tableSize { 0 };
    size_t m_count { 0 };
    IAllocator* m_pAlloc { nullptr };

    HashMap(IAllocator* _pAlloc = &g_Allocator);

    void Free();

    template<typename F>
    void Free(F&& freeNode);

    V& Add(const K& key, const V& value);

    V* Get(const K& key);

    V& GetOrAdd(const K& key);

    V& operator[](const K& key);

    void Erase(const K& key);

    template<typename F>
    void Erase(const K& key, F&& freeNode);

    void Rehash(size_t requiredTableSize);
};
