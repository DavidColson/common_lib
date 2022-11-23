#pragma once

void* MallocTrack(size_t size);
void* ReallocTrack(void* ptr, size_t size);
void FreeTrack(void* ptr);

int ReportMemoryLeaks();

