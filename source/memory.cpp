// Copyright 2020-2022 David Colson. All rights reserved.

Arena* g_pArenaFrame = nullptr;
Arena* g_pArenaPermenant = nullptr;

// ***********************************************************************

u64 Align(u64 toAlign, u64 alignment) {
    // Rounds up to nearest multiple of alignment (works if alignment is power of 2)
    return (toAlign + alignment - 1) & ~(alignment - 1);
}

// ***********************************************************************

u8* AlignPtr(u8* toAlign, u64 alignment) {
    return (u8*)Align(u64(toAlign), alignment);
}

// ***********************************************************************

Arena* ArenaCreate(i64 defaultReserve, bool noTrack) {
	SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    i64 reserveSize = Align(defaultReserve, sysInfo.dwPageSize);
    u8* pMemory = (u8*)VirtualAlloc(nullptr, reserveSize, MEM_RESERVE, PAGE_READWRITE);

	// commit the first page for the header
    u64 firstPageSize = Align(sizeof(Arena), sysInfo.dwPageSize);
    VirtualAlloc(pMemory, firstPageSize, MEM_COMMIT, PAGE_READWRITE);

    Assert(pMemory != nullptr);

#ifdef MEMORY_TRACKING
	if (!noTrack)
		CheckMalloc(pMemory, firstPageSize);
#endif

	Arena* pArena = (Arena*)pMemory; 

	pArena->pageSize = sysInfo.dwPageSize;
	pArena->reserveSize = reserveSize;
	pArena->pMemoryBase = pMemory + sizeof(Arena);
    pArena->pFirstUncommittedPage = pMemory + firstPageSize;
    pArena->pAddressLimit = pMemory + pArena->reserveSize;
    pArena->pCurrentHead = pArena->pMemoryBase;
    pArena->noTrack = noTrack;
	return pArena;
}

// ***********************************************************************

void ArenaReset(Arena* pArena) {
	Assert(pArena);

	if (pArena->pMemoryBase == nullptr)
        return;

#ifdef STAMP_RESET_ARENAS
	memset(pArena->pMemoryBase, 0xcc, pArena->pCurrentHead - pArena->pMemoryBase);
#endif

    pArena->pCurrentHead = pArena->pMemoryBase;
}

// ***********************************************************************

void ArenaFinished(Arena* pArena) {
	Assert(pArena);

	if (pArena->pMemoryBase != nullptr) {
#ifdef MEMORY_TRACKING
	if (!pArena->noTrack)
        CheckFree(pArena);
#endif
        VirtualFree(pArena, 0, MEM_RELEASE);
    }
}

// ***********************************************************************

void ArenaExpandCommitted(Arena* pArena, u8* pDesiredEnd) {
	Assert(pArena);

	i64 currentSpace = pArena->pFirstUncommittedPage - pArena->pMemoryBase;
    i64 requiredSpace = pDesiredEnd - pArena->pFirstUncommittedPage;
    Assert(requiredSpace > 0);
	Assert(requiredSpace < pArena->reserveSize); // block runaway memory leaks

    u64 size = Align(requiredSpace, pArena->pageSize);
    VirtualAlloc(pArena->pFirstUncommittedPage, size, MEM_COMMIT, PAGE_READWRITE);
#ifdef MEMORY_TRACKING
	if (!pArena->noTrack)
		CheckRealloc(pArena->pMemoryBase, pArena->pMemoryBase, currentSpace + size, currentSpace);
#endif
    pArena->pFirstUncommittedPage += size;
}


// ***********************************************************************

void* ArenaAlloc(Arena* pArena, i64 size, i64 align, bool uninitialized) {
	Assert(pArena);

	void* pMemory = nullptr;

	pMemory = AlignPtr(pArena->pCurrentHead, align);
	u8* pEnd = (u8*)pMemory + size;

	if (pEnd > pArena->pFirstUncommittedPage) {
		Assert(pEnd < pArena->pAddressLimit);
		ArenaExpandCommitted(pArena, pEnd);
	}

	pArena->pCurrentHead = pEnd;

	if (!uninitialized) 
		memset(pMemory, 0, size);
	return pMemory;
}

// ***********************************************************************

void* ArenaRealloc(Arena* pArena, void* ptr, i64 size, i64 oldSize, i64 align, bool uninitialized) {
	Assert(pArena);

	// easiest case, reduction in size, just give it back what it asked for
	// we memset the unused memory now to prevent use after frees
	if (size < oldSize) {
		char* pStart = (char*)ptr + size;
		memset(pStart, 0, oldSize - size);
		return ptr;
	}

	// if the current head is in fact the end of the current allocation, then you can just move the head
	u8* pCurrentEnd = (u8*)ptr + oldSize;
	if (pCurrentEnd == pArena->pCurrentHead) {
		u8* pNewEnd = (u8*)ptr + size;
		if (pNewEnd > pArena->pFirstUncommittedPage) {
			Assert(pNewEnd < pArena->pAddressLimit);
			ArenaExpandCommitted(pArena, pNewEnd);
		}

		if (!uninitialized) 
			memset(pArena->pCurrentHead, 0, pNewEnd - pCurrentEnd);
		pArena->pCurrentHead = pNewEnd;
		return ptr;
	}

	// worst case, move the memory
	u8* pOutput = (u8*)ArenaAlloc(pArena, size, align, false);
	if (!uninitialized) 
		memset(pOutput, 0, size);
	memcpy(pOutput, ptr, oldSize);
	return (void*)pOutput;
}

// ***********************************************************************

void* RawAlloc(i64 size, bool uninitialized) {
    void* pMemory = malloc(size);
#ifdef MEMORY_TRACKING
	CheckMalloc(pMemory, size);
#endif
	if (!uninitialized) 
		memset(pMemory, 0, size);
    return pMemory;
}

// ***********************************************************************

void* RawRealloc(void* ptr, i64 size, i64 oldSize, bool uninitialized) {
	void* pMemory = realloc(ptr, size);
#ifdef MEMORY_TRACKING
    CheckRealloc(pMemory, ptr, size, oldSize);
#endif
	if (!uninitialized && size > oldSize) {
		char* pStart = (char*)pMemory + oldSize;
		memset(pStart, 0, size - oldSize); 
	}
    return pMemory;
}

// ***********************************************************************

void RawFree(void* ptr) {
#ifdef MEMORY_TRACKING
    CheckFree(ptr);
#endif
    free(ptr);
}
