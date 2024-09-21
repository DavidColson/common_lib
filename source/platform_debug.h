// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

#include "memory.h"

struct String;

namespace PlatformDebug {
usize CollectStackTrace(void** stackFramesArray, usize arraySize, usize framesToSkip = 0);

String PrintStackTraceToString(void** stackFramesArray, usize nframes, Arena* pArena);
}
