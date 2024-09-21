// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

#include "memory.h"

struct String;

String DecodeBase64(Arena* pArena, String const& encodedString);

String EncodeBase64(Arena* pArena, usize length, const ubyte* bytes);
