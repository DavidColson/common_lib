// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

// ************************************************
// Arenas
// ************************************************

#define DEFAULT_RESERVE 268435456  // 256 megas

u64 Align(u64 toAlign, u64 alignment);
u8* AlignPtr(u8* toAlign, u64 alignment);

struct Arena {
	const char* name;
	i64 pageSize;
	i64 reserveSize;

	u8* pMemoryBase;
	u8* pCurrentHead;
	u8* pFirstUncommittedPage;
	u8* pAddressLimit;
	bool noTrack;
};

// Global shared arena's. You must create and release these yourself
// TODO: I think for safest usage, there should be no use of these inside commonlib itself, always take an arena param so the 
// application can decide how to use these
// In fact you might want to move these pointers outside commonlib entirely
extern Arena* g_pArenaFrame;
extern Arena* g_pArenaPermenant;

Arena* ArenaCreate(i64 defaultReserve = DEFAULT_RESERVE, bool noTrack=false);
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
inline void* operator new(u64, NewWrapper, void* ptr) {
    return ptr;
}
inline void operator delete(void*, NewWrapper, void*) {}
#define PlacementNew(ptr) new (NewWrapper(), ptr)

void* RawAlloc(i64 size, bool uninitialized = false);
void* RawRealloc(void* ptr, i64 size, i64 oldSize, bool uninitialized = false);
void RawFree(void* ptr);

#define RawNew1(type) (type*)RawAlloc(sizeof(type))
#define RawNew2(type, count) (type*)RawAlloc(sizeof(type)*count)
#define RawNew3(type, count, uninit) (type*)RawAlloc(sizeof(type)*count, uninit)
#define Get4thArg(a,b,c,d,...) d

// As with Arena's this macro should be used as much as possible for raw allocations
#define RawNew(...) Get4thArg(__VA_ARGS__,RawNew3,RawNew2,RawNew1)(__VA_ARGS__)
