// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

void* MallocWrap(u64 size);
void* ReallocWrap(void* ptr, u64 size, u64 oldSize);
void FreeWrap(void* ptr);

void CheckMalloc(void* pAllocated, u64 size);
void CheckRealloc(void* pAllocated, void* ptr, u64 size, u64 oldSize);
void CheckFree(void* ptr);

void MarkNotALeak(void* ptr);
int ReportMemoryLeaks();
void ReportMemoryUsage();
