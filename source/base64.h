// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

#include "light_string.h"

String DecodeBase64(String const& encodedString, IAllocator* pAlloc = &gAllocator);

String EncodeBase64(size_t length, const char* bytes, IAllocator* pAlloc = &gAllocator);
