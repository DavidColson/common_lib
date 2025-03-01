// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

struct String;

String DecodeBase64(Arena* pArena, String const& encodedString);

String EncodeBase64(Arena* pArena, u64 length, const u8* bytes);
