// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

#include "types.h"

void* MallocWrap(usize size);
void* ReallocWrap(void* ptr, usize size, usize oldSize);
void FreeWrap(void* ptr);

void CheckMalloc(void* pAllocatorPtr, void* pAllocated, usize size);
void CheckRealloc(void* pAllocatorPtr, void* pAllocated, void* ptr, usize size, usize oldSize);
void CheckFree(void* pAllocatorPtr, void* ptr);

void MarkNotALeak(void* ptr);
int ReportMemoryLeaks();
