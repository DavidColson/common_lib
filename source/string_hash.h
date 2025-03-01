// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

// Implements compile time string hashing.
// Forgive my academic masturbation here but this is a damn cool thing to have,
struct Fnv1a {
    constexpr static inline u32 Hash(char const* const aString, const u32 val = 0x811C9DC5) {
        return (aString[0] == '\0') ? val : Hash(&aString[1], (val ^ u32(aString[0])) * 0x01000193);
    }

    constexpr static inline u32 Hash(char const* const aString, const u64 aStrlen, const u32 val = 0x811C9DC5) {
        return (aStrlen == 0) ? val : Hash(aString + 1, aStrlen - 1, (val ^ u32(aString[0])) * 0x01000193);
    }
};

inline constexpr u32 operator"" _hash(const char* aString, const u64 aStrlen) {
    return Fnv1a::Hash(aString, aStrlen);
}
