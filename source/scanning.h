// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

namespace Scan {
// Scanning utilities
///////////////////////

struct ScanningState {
    const char* m_pTextStart;
    const char* m_pTextEnd;
    char* m_pCurrent { nullptr };
    char* m_pTokenStart { nullptr };
    char* m_pCurrentLineStart { nullptr };
    int m_line { 1 };
    bool m_encounteredError { false };
};

inline char Advance(ScanningState& scan) {
    return *(scan.m_pCurrent++);
}

inline char Peek(ScanningState& scan) {
    return *(scan.m_pCurrent);
}

inline bool IsAtEnd(ScanningState& scan) {
    return scan.m_pCurrent >= scan.m_pTextEnd;
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
}
