// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

struct Arena;
struct String;

namespace Scan {

// Scanning/Parsing utilities
///////////////////////

struct ScanningState {
    const char* pTextStart;
    const char* pTextEnd;
    char* pCurrent { nullptr };
    char* pTokenStart { nullptr };
    char* pCurrentLineStart { nullptr };
    int line { 1 };
    bool encounteredError { false };
};

inline char Advance(ScanningState& scan) {
    return *(scan.pCurrent++);
}

inline char Peek(ScanningState& scan) {
    return *(scan.pCurrent);
}

inline bool IsAtEnd(ScanningState& scan) {
    return scan.pCurrent >= scan.pTextEnd;
}

bool Match(ScanningState& scan, char expected);

char PeekNext(ScanningState& scan);

bool IsWhitespace(char c);

void AdvanceOverWhitespace(ScanningState& scan);

bool IsPartOfNumber(char c);

bool IsDigit(char c);

bool IsHexDigit(char c);

bool IsAlpha(char c);

bool IsAlphaNumeric(char c);

String ParseString(Arena* pArena, ScanningState& scan, char bound);

f64 ParseNumber(ScanningState& scan);

}
