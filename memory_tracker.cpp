#include "memory_tracker.h"

#include "string.h"
#include "hashmap.h"
#include "resizable_array.h"

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

	if (Allocation* alloc = pCtx->allocationTable.Get(ptr)) { // pre-existing allocation
		if (alloc->pointer != pMemory) { // Memory has changed location, so we must change the key
			// old alloc is effectively freed
			alloc->isLive = false;
			alloc->allocStackTraceFrames = CaptureStackBackTrace(1, 100, alloc->freeStackTrace, nullptr);

			Allocation newAlloc;
			newAlloc.pointer = pMemory;
			newAlloc.size = size;
			newAlloc.isLive = true;
			newAlloc.allocStackTraceFrames = CaptureStackBackTrace(1, 100, newAlloc.allocStackTrace, nullptr);
			pCtx->allocationTable.Erase(alloc->pointer);
			pCtx->allocationTable[pMemory] = newAlloc;
		}
		else
		{
			alloc->size = size;
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

	if (Allocation* alloc = pCtx->allocationTable.Get(ptr)) {
		if (!alloc->isLive)
			__debugbreak(); // Double free

		alloc->isLive = false;
		alloc->freeStackTraceFrames = CaptureStackBackTrace(1, 100, alloc->freeStackTrace, nullptr);
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

	for (size_t i = 0; i < pCtx->allocationTable.tableSize; i++)
	{
		if (pCtx->allocationTable.pTable[i].hash == UNUSED_HASH) {
			i++; continue;
		}
		Allocation& alloc = pCtx->allocationTable.pTable[i].value;
		if (alloc.isLive) {
			leakCounter++;
			printf("\n------ Oi dimwit, detected memory leak at address %p of size %zi. Fix your shit!\n", alloc.pointer, alloc.size);
			uint32_t nFrames = alloc.allocStackTraceFrames;

			ResizableArray<String, ForceNoTrackAllocator> stackFuncs;
			ResizableArray<String, ForceNoTrackAllocator> stackFiles;
			ResizableArray<size_t, ForceNoTrackAllocator> stackLines;

			size_t longestName = 0;
			for (uint32_t j = 0; j < nFrames-6; j++) {
				if (!SymFromAddr(process, (DWORD64)(alloc.allocStackTrace[j]), 0, symbol)) {
					DWORD d = GetLastError();
				}
				
				IMAGEHLP_LINE line;
				SymGetLineFromAddr(process, (DWORD64)(alloc.allocStackTrace[j]), &displacement, &line);

				size_t len = strlen(symbol->Name);
				if (len > longestName)
					longestName = len;

				stackFuncs.PushBack(CopyCString(symbol->Name, true, ForceNoTrackAllocator()));
				stackFiles.PushBack(CopyCString(line.FileName, true, ForceNoTrackAllocator()));
				stackLines.PushBack((size_t)line.LineNumber);
			}

			for (uint32_t j = 0; j < nFrames-6; j++) {
				printf(" %-*s %s:%zi\n", (int)longestName, stackFuncs[j].pData, stackFiles[j].pData, stackLines[j]);
			}

			stackFuncs.Free([] (String& str) {
				FreeString(str, ForceNoTrackAllocator());
			});
			stackFiles.Free([] (String& str) {
				FreeString(str, ForceNoTrackAllocator());
			});
			stackLines.Free();
		}
	}

	if (leakCounter > 0)
		printf("\n");
	return leakCounter;
}