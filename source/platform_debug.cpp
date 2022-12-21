#include "platform_debug.h"

#include "memory.h"
#include "resizable_array.h"
#include "string_builder.h"

#include <Windows.h>
#include <dbghelp.h>

#ifdef _WIN32
namespace PlatformDebug {
size_t CollectStackTrace(void** stackFramesArray, size_t arraySize, size_t framesToSkip) {
    return CaptureStackBackTrace(DWORD(1 + framesToSkip), (DWORD)arraySize, stackFramesArray, nullptr);
}

String PrintStackTraceToString(void** stackFramesArray, size_t nframes, IAllocator* pAlloc) {
    HANDLE process = GetCurrentProcess();
    SymInitialize(process, nullptr, true);

    // We allocate space for the symbol info and space for the name string
    SYMBOL_INFO* symbol;
    symbol = (SYMBOL_INFO*)pAlloc->Allocate(sizeof(SYMBOL_INFO) + 256 * sizeof(char));
    memset(symbol, 0, sizeof(SYMBOL_INFO) + 256 * sizeof(char));
    symbol->MaxNameLen = 255;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    DWORD displacement;

    ResizableArray<String> stackFuncs(pAlloc);
    ResizableArray<String> stackFiles(pAlloc);
    ResizableArray<size_t> stackLines(pAlloc);

    size_t longestName = 0;
    for (uint32_t j = 0; j < nframes - 6; j++) {
        if (!SymFromAddr(process, (DWORD64)(stackFramesArray[j]), 0, symbol)) {
            DWORD d = GetLastError();
        }

        IMAGEHLP_LINE line;
        SymGetLineFromAddr(process, (DWORD64)(stackFramesArray[j]), &displacement, &line);

        size_t len = strlen(symbol->Name);
        if (len > longestName)
            longestName = len;

        stackFuncs.PushBack(CopyCString(symbol->Name, pAlloc));
        stackFiles.PushBack(CopyCString(line.FileName, pAlloc));
        stackLines.PushBack((size_t)line.LineNumber);
    }

    StringBuilder builder;
    for (uint32_t j = 0; j < nframes - 6; j++) {
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
