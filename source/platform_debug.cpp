// Copyright 2020-2022 David Colson. All rights reserved.

#include "platform_debug.h"

#include "types.h"
#include "memory.h"
#include "resizable_array.inl"
#include "string_builder.h"
#include "light_string.h"

// Windows header defines
// ***********************************************************************

extern "C" {
#define WIN(r) __declspec(dllimport) r __stdcall

typedef void* WinHandle;
WIN(u32) GetLastError();
WIN(u16) RtlCaptureStackBackTrace(u32 FramesToSkip, u32 FramesToCapture, void** BackTrace, u32* BackTraceHash);
#define CaptureStackBackTrace RtlCaptureStackBackTrace

WinHandle GetCurrentProcess();

typedef struct _SYMBOL_INFO {
    u32       SizeOfStruct;
    u32       TypeIndex;        // Type Index of symbol
    u64       Reserved[2];
    u32       Index;
    u32       Size;
    u64       ModBase;          // Base Address of module comtaining this symbol
    u32       Flags;
    u64       Value;            // Value of symbol, ValuePresent should be 1
    u64       Address;          // Address of symbol including base address of module
    u32       Register;         // register holding value or pointer to value
    u32       Scope;            // scope of the symbol
    u32       Tag;              // pdb classification
    u32       NameLen;          // Actual length of name
    u32       MaxNameLen;
    char      Name[1];          // Name of symbol
} SYMBOL_INFO, *PSYMBOL_INFO;

typedef struct _IMAGEHLP_LINE64 {
    u32      SizeOfStruct;           // set to sizeof(IMAGEHLP_LINE64)
    void*    Key;                    // internal
    void*    LineNumber;             // line number in file
    char*    FileName;               // full filename
    u64      Address;                // first instruction of line
} IMAGEHLP_LINE64, *PIMAGEHLP_LINE64;
#define IMAGEHLP_LINE IMAGEHLP_LINE64

WIN(i32) SymInitialize(WinHandle hProcess, const char* UserSearchPath, i32 fInvadeProcess);
WIN(i32) SymFromAddr(WinHandle hProcess, u64 Address, u64* Displacement, PSYMBOL_INFO Symbol);
WIN(i32) SymGetLineFromAddr64( WinHandle hProcess, u64 qwAddr, u32* pdwDisplacement, PIMAGEHLP_LINE64 Line64);
#define SymGetLineFromAddr SymGetLineFromAddr64
}

#ifdef _WIN32
namespace PlatformDebug {

// ***********************************************************************

usize CollectStackTrace(void** stackFramesArray, usize arraySize, usize framesToSkip) {
    return CaptureStackBackTrace(u32(1 + framesToSkip), (u32)arraySize, stackFramesArray, nullptr);
}

// ***********************************************************************

String PrintStackTraceToString(void** stackFramesArray, usize nframes, IAllocator* pAlloc) {
    WinHandle process = GetCurrentProcess();
    SymInitialize(process, nullptr, true);

    // We allocate space for the symbol info and space for the name string
    SYMBOL_INFO* symbol;
    symbol = (SYMBOL_INFO*)pAlloc->Allocate(sizeof(SYMBOL_INFO) + 256 * sizeof(byte));
    memset(symbol, 0, sizeof(SYMBOL_INFO) + 256 * sizeof(byte));
    symbol->MaxNameLen = 255;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    u32 displacement;

    ResizableArray<String> stackFuncs(pAlloc);
    ResizableArray<String> stackFiles(pAlloc);
    ResizableArray<usize> stackLines(pAlloc);

    usize longestName = 0;
    for (u32 j = 0; j < nframes - 6; j++) {
        if (!SymFromAddr(process, (u64)(stackFramesArray[j]), 0, symbol)) {
            u32 d = GetLastError();
        }

        IMAGEHLP_LINE line;
        SymGetLineFromAddr(process, (u64)(stackFramesArray[j]), &displacement, &line);

        usize len = strlen(symbol->Name);
        if (len > longestName)
            longestName = len;

        stackFuncs.PushBack(CopyCString(symbol->Name, pAlloc));
        stackFiles.PushBack(CopyCString(line.FileName, pAlloc));
        stackLines.PushBack((usize)line.LineNumber);
    }

    StringBuilder builder(pAlloc);
    for (u32 j = 0; j < nframes - 6; j++) {
        builder.AppendFormat(" %-*s %s:%zi\n", (int)longestName, stackFuncs[j].pData, stackFiles[j].pData, stackLines[j]);
    }
    String output = builder.CreateString(true, pAlloc);

    stackFuncs.Free([&pAlloc](String& str) {
        FreeString(str, pAlloc);
    });
    stackFiles.Free([&pAlloc](String& str) {
        FreeString(str, pAlloc);
    });
    stackLines.Free();
    return output;
}
}
#endif
