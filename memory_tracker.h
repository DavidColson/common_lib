#pragma once

void* mallocTrack(size_t size);
void* reallocTrack(void* ptr, size_t size);
void freeTrack(void* ptr);

