// Copyright 2020-2022 David Colson. All rights reserved.

#ifdef _WIN32

namespace PlatformDebug {

// ***********************************************************************

u64 CollectStackTrace(void** stackFramesArray, u64 arraySize, u64 framesToSkip) {
    return CaptureStackBackTrace(u32(1 + framesToSkip), (u32)arraySize, stackFramesArray, nullptr);
}

// ***********************************************************************

String PrintStackTraceToString(void** stackFramesArray, u64 nframes, Arena* pArena) {
	Assert(pArena);

    HANDLE process = GetCurrentProcess();
    SymInitialize(process, nullptr, true);

    // We allocate space for the symbol info and space for the name string
    SYMBOL_INFO* symbol;
    symbol = (SYMBOL_INFO*)ArenaAlloc(pArena, sizeof(SYMBOL_INFO) + 256 * sizeof(byte), alignof(byte), false);
    memset(symbol, 0, sizeof(SYMBOL_INFO) + 256 * sizeof(byte));
    symbol->MaxNameLen = 255;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    DWORD displacement;

    ResizableArray<String> stackFuncs(pArena);
    ResizableArray<String> stackFiles(pArena);
    ResizableArray<u64> stackLines(pArena);

    u64 longestName = 0;
    for (u32 j = 0; j < nframes - 6; j++) {
        if (!SymFromAddr(process, (u64)(stackFramesArray[j]), 0, symbol)) {
            u32 d = GetLastError();
        }

        IMAGEHLP_LINE line;
        SymGetLineFromAddr(process, (u64)(stackFramesArray[j]), &displacement, &line);

        u64 len = strlen(symbol->Name);
        if (len > longestName)
            longestName = len;

        stackFuncs.PushBack(CopyCString(symbol->Name, pArena));
        stackFiles.PushBack(CopyCString(line.FileName, pArena));
        stackLines.PushBack(line.LineNumber);
    }

    StringBuilder builder(pArena);
    for (u32 j = 0; j < nframes - 6; j++) {
        builder.AppendFormat(" %-*s %s:%d\n", (int)longestName, stackFuncs[j].pData, stackFiles[j].pData, stackLines[j]);
    }
    String output = builder.CreateString(pArena, true);
    return output;
}

}
#endif
