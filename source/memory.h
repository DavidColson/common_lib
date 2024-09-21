// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

#include "types.h"

// ************************************************
// Arenas
// ************************************************

#define DEFAULT_RESERVE 268435456  // 256 megabytes

usize Align(usize toAlign, usize alignment);
u8* AlignPtr(u8* toAlign, usize alignment);

struct Arena {
	const char* name;
	i64 pageSize;
	i64 reserveSize;

	u8* pMemoryBase;
	u8* pCurrentHead;
	u8* pFirstUncommittedPage;
	u8* pAddressLimit;
};

// cleared once per frame
static Arena gArenaTemp;

// cleared on program shutdown
static Arena gArenaPermenant;

void ArenaInit(Arena* pArena, i64 defaultReserve = DEFAULT_RESERVE);
void ArenaReset(Arena* pArena);
void ArenaFinished(Arena* pArena);
void ArenaExpandCommitted(Arena* pArena, u8* pDesiredEnd);

// Core arena allocation functions. Ostensibly everything should come through here, with minimal use of raw allocation
void* ArenaAlloc(Arena* arena, i64 size, i64 align, bool uninitialized = false); 
void* ArenaRealloc(Arena* arena, void* ptr, i64 newSize, i64 oldSize, i64 align, bool uninitialized = false); 

#define New2(pArena, type) (type*)ArenaAlloc(pArena, sizeof(type), alignof(type), false)
#define New3(pArena, type, count) (type*)ArenaAlloc(pArena, sizeof(type)*count, alignof(type), false)
#define New4(pArena, type, count, uninit) (type*)ArenaAlloc(pArena, sizeof(type)*count, alignof(type), uninit)
#define Get5thArg(a,b,c,d,e,...) e

// This is the primary allocation function you should be using for most things, 
// it just simplifies the parameters to the above alloc function
#define New(...) Get5thArg(__VA_ARGS__,New4,New3,New2)(__VA_ARGS__)


// ************************************************
// Raw (system) allocators
// ************************************************

struct NewWrapper {};
inline void* operator new(usize, NewWrapper, void* ptr) {
    return ptr;
}
inline void operator delete(void*, NewWrapper, void*) {}
#define PlacementNew(ptr) new (NewWrapper(), ptr)

void* RawAlloc(i64 size, bool uninitialized = false);
void* RawRealloc(void* ptr, i64 size, i64 oldSize, bool uninitialized = false);
void RawFree(void* ptr);

#define RawNew2(pArena, type) (type*)RawAlloc(sizeof(type))
#define RawNew3(pArena, type, count) (type*)RawAlloc(sizeof(type)*count)
#define RawNew4(pArena, type, count, uninit) (type*)RawAlloc(pArena, sizeof(type)*count, uninit)

// As with Arena's this macro should be used as much as possible for raw allocations
#define RawNew(...) Get5thArg(__VA_ARGS__,New4,New3,New2)(__VA_ARGS__)
