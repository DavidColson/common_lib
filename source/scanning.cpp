// Copyright 2020-2022 David Colson. All rights reserved.

#include "scanning.h"

#include <ctype.h>

// Scanning utilities
///////////////////////

// TODO: Consider making these part of string

// ***********************************************************************

bool Scan::Match(ScanningState& scan, char expected) {
    if (*(scan.m_pCurrent) == expected) {
        Advance(scan);
        return true;
    }
    return false;
}

// ***********************************************************************

char Scan::PeekNext(ScanningState& scan) {
    return *(scan.m_pCurrent++);
}

// ***********************************************************************

bool Scan::IsWhitespace(char c) {
    if (c == ' ' || c == '\r' || c == '\t')
        return true;
    return false;
}

// ***********************************************************************

void Scan::AdvanceOverWhitespace(ScanningState& scan) {
    char c = *(scan.m_pCurrent);
    while (IsWhitespace(c)) {
        Advance(scan);
    }
}

// ***********************************************************************

bool Scan::IsPartOfNumber(char c) {
    return (c >= '0' && c <= '9') || c == '-' || c == '+' || c == '.';
}

// ***********************************************************************

bool Scan::IsDigit(char c) {
    return (c >= '0' && c <= '9');
}

// ***********************************************************************

bool Scan::IsHexDigit(char c) {
    return isxdigit(c);
}

// ***********************************************************************

bool Scan::IsAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

// ***********************************************************************

bool Scan::IsAlphaNumeric(char c) {
    return IsAlpha(c) || IsDigit(c);
}
