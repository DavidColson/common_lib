// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

// String Class
// --------------------
// NOT null terminated string
// The functions in this struct guarantee to do zero memory operations
// and is fully POD, trivially copyable

struct String {
	char* pData = nullptr;
    i64 length = 0;

    // TODOs
    // [ ] contains/nocase
    // [ ] Tof32/ToInt
    // [ ] trimRight/Left

    String() {}

    String(const char* str);

    void operator=(const char* str);

    bool operator==(const String& other) const;

    bool operator==(const char* other) const;

    bool operator!=(const String& other) const;

    bool operator!=(const char* other) const;

    char operator[](i64 index) const;

    String SubStr(i64 start, i64 len = -1);
	
	// find substr in self, returns the index it was found, or -1
	i64 Find(String substr);
};

String CopyCString(const char* string, Arena* pArena);

String CopyCStringRange(char* start, char* end, Arena* pArena);

String CopyString(String& string, Arena* pArena);

String AllocString(u64 length, Arena* pArena);

