// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

#include "types.h"
#include "memory.h"

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
constexpr bool is_enuv = is_enum<T>::value;

// utility to disable the generic template specialization that is
// used for enum types only.
template<typename T, bool Enabled>
struct EnableHashIf {};

template<typename T>
struct EnableHashIf<T, true> {
    usize operator()(const T& p) const {
        return usize(p);
    }
};
}

template<typename K>
struct KeyFuncs : Internal::EnableHashIf<K, Internal::is_enuv<K>> {
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
struct KeyFuncs<byte> {
    u64 Hash(byte key) const;
    bool Cmp(byte key1, byte key2) const;
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
struct KeyFuncs<byte*> {
    u64 Hash(const byte* key) const;
    bool Cmp(const byte* key1, const byte* key2) const;
};

template<>
struct KeyFuncs<const byte*> {
    u64 Hash(const byte* key) const;
    bool Cmp(const byte* key1, const byte* key2) const;
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
// Open addressing to reduce memory allocs and improve cache coherency, requires manual freeing
// TODO Documentation etca

template<typename K, typename V, typename KF = KeyFuncs<K>>
struct HashMap {
    HashNode<K, V>* pTable { nullptr };
    KF keyFuncs;
    size tableSize { 0 };
    size count { 0 };
    IAllocator* pAlloc { nullptr };

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

    void Rehash(size requiredTableSize);
};
