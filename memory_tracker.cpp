#include "memory_tracker.h"

#include "string.h"
#include "hashmap.h"
#include "resizable_array.h"

#include "Windows.h"
#include "dbghelp.h"

struct ForceNoTrackAllocator : public IAllocator {
	void* 	Allocate(size_t size) { return malloc(size); }
	void* 	Reallocate(void* ptr, size_t size, size_t oldSize) { return realloc(ptr, size); }
	void 	Free(void* ptr) { free(ptr); }
};

struct Allocation {
	void* pointer { nullptr };
	void* pAllocator { nullptr };
	size_t size { 0 };
	bool isLive { false };
	void* allocStackTrace[100];
	uint32_t allocStackTraceFrames { 0 };
	void* freeStackTrace[100];
	uint32_t freeStackTraceFrames { 0 };
};

struct MemoryTrackerState {
	HashMap<void*, Allocation> allocationTable;
};

namespace {
	ForceNoTrackAllocator noTrackAllocator;
	MemoryTrackerState* pCtx { nullptr };
};

void InitContext() {
	pCtx = (MemoryTrackerState*)malloc(sizeof(MemoryTrackerState));
	SYS_P_NEW(pCtx) MemoryTrackerState();
	pCtx->allocationTable.pAlloc = &noTrackAllocator;
}

void* MallocWrap(size_t size) {
	void* pMemory = malloc(size);
	CheckMalloc(nullptr, pMemory, size);
	return pMemory;
}

void* ReallocWrap(void* ptr, size_t size, size_t oldSize) {
	void* pMemory = realloc(ptr, size);
	CheckRealloc(nullptr, pMemory, ptr, size, oldSize);
	return pMemory;
}

void FreeWrap(void* ptr) {
	CheckFree(nullptr, ptr);
	free(ptr);
}

void CheckMalloc(void* pAllocatorPtr, void* pAllocated, size_t size) {
	if (pCtx == nullptr) InitContext();

	Allocation allocation;
	allocation.pointer = pAllocated;
	allocation.pAllocator = pAllocatorPtr;
	allocation.size = size;
	allocation.isLive = true;
	allocation.allocStackTraceFrames = CaptureStackBackTrace(1, 100, allocation.allocStackTrace, nullptr);
	pCtx->allocationTable[pAllocated] = allocation;
}

void CheckRealloc(void* pAllocatorPtr, void* pAllocated, void* ptr, size_t size, size_t oldSize) {
	if (pCtx == nullptr) InitContext();

	if (Allocation* alloc = pCtx->allocationTable.Get(ptr)) { // pre-existing allocation
		if (alloc->pAllocator != pAllocatorPtr)
			__debugbreak();
		
		if (alloc->pointer != pAllocated) { // Memory has changed location, so we must change the key
			// old alloc is effectively freed
			alloc->isLive = false;
			alloc->allocStackTraceFrames = CaptureStackBackTrace(1, 100, alloc->freeStackTrace, nullptr);

			Allocation newAlloc;
			newAlloc.pointer = pAllocated;
			newAlloc.pAllocator = pAllocatorPtr;
			newAlloc.size = size;
			newAlloc.isLive = true;
			newAlloc.allocStackTraceFrames = CaptureStackBackTrace(1, 100, newAlloc.allocStackTrace, nullptr);
			pCtx->allocationTable.Erase(alloc->pointer);
			pCtx->allocationTable[pAllocated] = newAlloc;
		}
		else
		{
			alloc->size = size;
		}
	}
	else { // new allocation
		Allocation allocation;
		allocation.pointer = pAllocated;
		allocation.pAllocator = pAllocatorPtr;
		allocation.size = size;
		allocation.isLive = true;
		allocation.allocStackTraceFrames = CaptureStackBackTrace(1, 100, allocation.allocStackTrace, nullptr);
		pCtx->allocationTable[pAllocated] = allocation;
	}
}

void PrintStackTrace(void** stackTrace, uint32_t stackDepth) {
	HANDLE process = GetCurrentProcess();
	SymInitialize(process, nullptr, true);

	// We allocate space for the symbol info and space for the name string
	SYMBOL_INFO* symbol;
	symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
	symbol->MaxNameLen = 255;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	DWORD displacement;

	ResizableArray<String> stackFuncs(&noTrackAllocator);
	ResizableArray<String> stackFiles(&noTrackAllocator);
	ResizableArray<size_t> stackLines(&noTrackAllocator);

	size_t longestName = 0;
	for (uint32_t j = 0; j < stackDepth-6; j++) {
		if (!SymFromAddr(process, (DWORD64)(stackTrace[j]), 0, symbol)) {
			DWORD d = GetLastError();
		}
				
		IMAGEHLP_LINE line;
		SymGetLineFromAddr(process, (DWORD64)(stackTrace[j]), &displacement, &line);

		size_t len = strlen(symbol->Name);
		if (len > longestName)
			longestName = len;

		stackFuncs.PushBack(CopyCString(symbol->Name, &noTrackAllocator));
		stackFiles.PushBack(CopyCString(line.FileName, &noTrackAllocator));
		stackLines.PushBack((size_t)line.LineNumber);
	}

	for (uint32_t j = 0; j < stackDepth-6; j++) {
		printf(" %-*s %s:%zi\n", (int)longestName, stackFuncs[j].pData, stackFiles[j].pData, stackLines[j]);
	}

	stackFuncs.Free([] (String& str) {
		FreeString(str, &noTrackAllocator);
	});
	stackFiles.Free([] (String& str) {
		FreeString(str, &noTrackAllocator);
	});
	stackLines.Free();
}

void ReportDoubleFree(Allocation& alloc, void** newFreeTrace, uint32_t newFreeTraceFrames) {
	printf("\n------ Hey idiot, detected double free at %p. Fix your shit! ------\n", alloc.pointer);
	printf("     Previously Freed at: \n");
	PrintStackTrace(alloc.freeStackTrace, alloc.freeStackTraceFrames);
	printf("     Freed again at: \n");
	PrintStackTrace(newFreeTrace, newFreeTraceFrames);
	__debugbreak();
}

void ReportUnknownFree(void* ptr) {
	printf("\n------ Hey idiot, detected free of untracked memory %p. Fix your shit! ------\n", ptr);
	__debugbreak();
}

void CheckFree(void* pAllocatorPtr, void* ptr) {
	if (pCtx == nullptr) InitContext();

	if (Allocation* alloc = pCtx->allocationTable.Get(ptr)) {
		if (alloc->pAllocator != pAllocatorPtr)
			__debugbreak(); // Alloc/Free allocator mismatch TODO: Report an error

		if (!alloc->isLive) {
			void* stackTrace[100];
			uint32_t stackFrames = CaptureStackBackTrace(1, 100, stackTrace, nullptr);
			ReportDoubleFree(*alloc, stackTrace, stackFrames);
		}

		alloc->isLive = false;
		alloc->freeStackTraceFrames = CaptureStackBackTrace(1, 100, alloc->freeStackTrace, nullptr);
	}
	else {
		ReportUnknownFree(ptr);
	}
}

int ReportMemoryLeaks() {
#ifdef MEMORY_TRACKING
	if (pCtx == nullptr)
		return 0;

	int leakCounter = 0;
	for (size_t i = 0; i < pCtx->allocationTable.tableSize; i++)
	{
		if (pCtx->allocationTable.pTable[i].hash != UNUSED_HASH) {
			Allocation& alloc = pCtx->allocationTable.pTable[i].value;
			if (alloc.isLive) {
				leakCounter++;
				printf("\n------ Oi dimwit, detected memory leak at address %p of size %zi. Fix your shit! ------\n", alloc.pointer, alloc.size);
				PrintStackTrace(alloc.allocStackTrace, alloc.allocStackTraceFrames);
			}
		}
	}

	if (leakCounter > 0)
		printf("\n");
	return leakCounter;	
#else
	return 0;
#endif
}