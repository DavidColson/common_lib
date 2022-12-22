// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

#include "memory.h"

struct String;

String DecodeBase64(String const& encodedString, IAllocator* pAlloc = &g_Allocator);

String EncodeBase64(size_t length, const char* bytes, IAllocator* pAlloc = &g_Allocator);
