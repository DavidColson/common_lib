// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

void* MallocWrap(u64 size);
void* ReallocWrap(void* ptr, u64 size, u64 oldSize);
void FreeWrap(void* ptr);

void CheckMalloc(void* pAllocatorPtr, void* pAllocated, u64 size);
void CheckRealloc(void* pAllocatorPtr, void* pAllocated, void* ptr, u64 size, u64 oldSize);
void CheckFree(void* pAllocatorPtr, void* ptr);

void MarkNotALeak(void* ptr);
int ReportMemoryLeaks();
