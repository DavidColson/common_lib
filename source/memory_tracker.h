// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

void* MallocWrap(size_t size);
void* ReallocWrap(void* ptr, size_t size, size_t oldSize);
void FreeWrap(void* ptr);

void CheckMalloc(void* pAllocatorPtr, void* pAllocated, size_t size);
void CheckRealloc(void* pAllocatorPtr, void* pAllocated, void* ptr, size_t size, size_t oldSize);
void CheckFree(void* pAllocatorPtr, void* ptr);

void MarkNotALeak(void* ptr);
int ReportMemoryLeaks();
