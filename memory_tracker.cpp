#include "memory_tracker.h"

#include "hashmap.h"
#include "array.h"

#include "Windows.h"
#include "dbghelp.h"

struct ForceNoTrackAllocator {
	ForceNoTrackAllocator() {}
	ForceNoTrackAllocator(const char* _name) {}

	void* 	Allocate(size_t size) { return malloc(size); }
	void* 	Reallocate(void* ptr, size_t size) { return realloc(ptr, size); }
	void 	Free(void* ptr) { free(ptr); }

	const char* GetName() { return ""; }
	void  		SetName(const char* _name) {}
};

struct Allocation {
	void* pointer { nullptr };
	size_t size { 0 };
	bool isLive { false };
	void* allocStackTrace[100];
	uint32_t allocStackTraceFrames { 0 };
	void* freeStackTrace[100];
	uint32_t freeStackTraceFrames { 0 };
};

struct MemoryTrackerState {
	HashMap<void*, Allocation, ForceNoTrackAllocator> allocationTable;
};

namespace {
	MemoryTrackerState* pCtx { nullptr };
};

void* MallocTrack(size_t size) {
	if (pCtx == nullptr)
	{
		pCtx = (MemoryTrackerState*)malloc(sizeof(MemoryTrackerState));
		SYS_P_NEW(pCtx) MemoryTrackerState();
	}

	void* pMemory = malloc(size);

	Allocation allocation;
	allocation.pointer = pMemory;
	allocation.size = size;
	allocation.isLive = true;
	allocation.allocStackTraceFrames = CaptureStackBackTrace(1, 100, allocation.allocStackTrace, nullptr);
	pCtx->allocationTable[pMemory] = allocation;

	return pMemory;
}

void* ReallocTrack(void* ptr, size_t size) {
	if (pCtx == nullptr)
	{
		pCtx = (MemoryTrackerState*)malloc(sizeof(MemoryTrackerState));
		SYS_P_NEW(pCtx) MemoryTrackerState();
	}

	void* pMemory = realloc(ptr, size);

	if (pCtx->allocationTable.Exists(ptr)) { // pre-existing allocation
		Allocation& alloc = pCtx->allocationTable[ptr];

		if (alloc.pointer != pMemory) { // Memory has changed location, so we must change the key
			// old alloc is effectively freed
			alloc.isLive = false;
			alloc.allocStackTraceFrames = CaptureStackBackTrace(1, 100, alloc.freeStackTrace, nullptr);

			Allocation newAlloc;
			newAlloc.pointer = pMemory;
			newAlloc.size = size;
			newAlloc.isLive = true;
			newAlloc.allocStackTraceFrames = CaptureStackBackTrace(1, 100, newAlloc.allocStackTrace, nullptr);
			pCtx->allocationTable.Erase(alloc.pointer);
			pCtx->allocationTable[pMemory] = newAlloc;
		}
		else
		{
			alloc.size = size;
		}
	}
	else { // new allocation
		Allocation allocation;
		allocation.pointer = pMemory;
		allocation.size = size;
		allocation.isLive = true;
		allocation.allocStackTraceFrames = CaptureStackBackTrace(1, 100, allocation.allocStackTrace, nullptr);
		pCtx->allocationTable[pMemory] = allocation;
		return pMemory;
	}

	return pMemory;
}

void FreeTrack(void* ptr) {
	if (pCtx == nullptr)
	{
		pCtx = (MemoryTrackerState*)malloc(sizeof(MemoryTrackerState));
		SYS_P_NEW(pCtx) MemoryTrackerState();
	}

	if (pCtx->allocationTable.Exists(ptr)) {
		Allocation& alloc = pCtx->allocationTable[ptr];

		if (!alloc.isLive)
			__debugbreak(); // Double free

		alloc.isLive = false;
		alloc.freeStackTraceFrames = CaptureStackBackTrace(1, 100, alloc.freeStackTrace, nullptr);
	}
	else {
		__debugbreak(); // Attempting to free some memory that was never allocated
	}
	free(ptr);
}

int ReportMemoryLeaks() {
	int leakCounter = 0;
	HANDLE process = GetCurrentProcess();
	SymInitialize(process, nullptr, true);

	// We allocate space for the symbol info and space for the name string
	SYMBOL_INFO* symbol;
	symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
	symbol->MaxNameLen = 255;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

	DWORD displacement;

	for (uint32_t i = 0; i < pCtx->allocationTable.bucketCount; i++)
	{
		HashNode<void*, Allocation>* pEntry = pCtx->allocationTable.pTable[i];
		while (pEntry != nullptr) {
			// Do your stuff in iteration
			if (pEntry->value.isLive) {
				leakCounter++;
				printf("\n------ Detected memory leak at address %p of size %zi\n", pEntry->value.pointer, pEntry->value.size);
				uint32_t nFrames = pEntry->value.allocStackTraceFrames;

				typedef BaseString<char, ForceNoTrackAllocator> NoTrackString;
				Array<NoTrackString, ForceNoTrackAllocator> stackFuncs;
				Array<NoTrackString, ForceNoTrackAllocator> stackFiles;
				Array<size_t, ForceNoTrackAllocator> stackLines;

				size_t longestName = 0;
				for (uint32_t j = 0; j < nFrames-6; j++) {
					if (!SymFromAddr(process, (DWORD64)(pEntry->value.allocStackTrace[j]), 0, symbol)) {
						DWORD d = GetLastError();
					}
					
					IMAGEHLP_LINE line;
					SymGetLineFromAddr(process, (DWORD64)(pEntry->value.allocStackTrace[j]), &displacement, &line);

					size_t len = strlen(symbol->Name);
					if (len > longestName)
						longestName = len;

					stackFuncs.PushBack(symbol->Name);
					stackFiles.PushBack(line.FileName);
					stackLines.PushBack((size_t)line.LineNumber);
				}

				for (uint32_t j = 0; j < nFrames-6; j++) {
					printf(" %-*s %s:%zi\n", (int)longestName, stackFuncs[j].pData, stackFiles[j].pData, stackLines[j]);
				}

			}
			pEntry = pEntry->pNext;
		}
	}

	if (leakCounter > 0)
		printf("\n");
	return leakCounter;
}