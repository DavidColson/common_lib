// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

struct String;

namespace Debug {
u64 CollectStackTrace(void** stackFramesArray, u64 arraySize, u64 framesToSkip = 0);
String PrintStackTraceToString(void** stackFramesArray, u64 nframes, Arena* pArena);
}
