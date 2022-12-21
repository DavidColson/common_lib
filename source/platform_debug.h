// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

#include "light_string.h"

namespace PlatformDebug {
size_t CollectStackTrace(void** stackFramesArray, size_t arraySize, size_t framesToSkip = 0);

String PrintStackTraceToString(void** stackFramesArray, size_t nframes, IAllocator* pAlloc = &gAllocator);
}
