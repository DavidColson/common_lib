// Copyright 2020-2022 David Colson. All rights reserved.

#include "scanning.h"

#include "light_string.h"
#include "memory.h"

#include <ctype.h>
#include <stdlib.h>

// Scanning utilities
///////////////////////

// ***********************************************************************

bool Scan::Match(ScanningState& scan, char expected) {
    if (*(scan.pCurrent) == expected) {
        Advance(scan);
        return true;
    }
    return false;
}

// ***********************************************************************

char Scan::PeekNext(ScanningState& scan) {
    return *(scan.pCurrent+1);
}

// ***********************************************************************

bool Scan::IsWhitespace(char c) {
    if (c == ' ' || c == '\r' || c == '\t')
        return true;
    return false;
}

// ***********************************************************************

void Scan::AdvanceOverWhitespace(ScanningState& scan) {
    while (IsWhitespace(*(scan.pCurrent))) {
		if (Advance(scan) == '\n') {
			scan.line++;
			scan.pCurrentLineStart = scan.pCurrent;
		}
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

// ***********************************************************************

String ParseStringSlow(Arena* pArena, Scan::ScanningState& scan, byte bound) {
    byte* start = scan.pCurrent;
    byte* pos = start;
    while (*pos != bound && !Scan::IsAtEnd(scan)) {
        pos++;
    }
    usize count = pos - start;
    byte* outputString = new byte[count * 2];  // to allow for escape chars TODO: Convert to Mallloc
    pos = outputString;

    byte* cursor = scan.pCurrent;

    for (usize i = 0; i < count; i++) {
        byte c = *(cursor++);

        // Disallowed characters
        switch (c) {
            case '\n': break;
            case '\r':
                if (Scan::Peek(scan) == '\n')  // CRLF line endings
                    cursor++;
                break;
                // Log::Crit("Unexpected end of line"); break;
            default: break;
        }

        if (c == '\\') {
            byte next = *(cursor++);
            switch (next) {
                // Convert basic escape sequences to their actual characters
                case '\'': *pos++ = '\''; break;
                case '"': *pos++ = '"'; break;
                case '\\': *pos++ = '\\'; break;
                case 'b': *pos++ = '\b'; break;
                case 'f': *pos++ = '\f'; break;
                case 'n': *pos++ = '\n'; break;
                case 'r': *pos++ = '\r'; break;
                case 't': *pos++ = '\t'; break;
                case 'v': *pos++ = '\v'; break;
                case '0': *pos++ = '\0'; break;

                // Unicode stuff, not doing this for now
                case 'u':
                    // Log::Crit("This parser does not yet support unicode escape codes"); break;

                // Line terminators, allowed but we do not include them in the final string
                case '\n': break;
                case '\r':
                    if (Scan::Peek(scan) == '\n')  // CRLF line endings
                        cursor++;
                    break;
                default:
                    *pos++ = next;  // all other escaped characters are kept as is, without the '\'
                                    // that preceeded it
            }
        } else
            *pos++ = c;
    }
    cursor++;

    scan.pCurrent = cursor;

    String result = CopyCStringRange(outputString, pos, pArena);
    delete outputString;
    return result;
}

// ***********************************************************************

String Scan::ParseString(Arena* pArena, Scan::ScanningState& scan, byte bound) {
    byte* start = scan.pCurrent;
    while (*(scan.pCurrent) != bound && !Scan::IsAtEnd(scan)) {
        if (*(scan.pCurrent++) == '\\') {
            scan.pCurrent = start;
            return ParseStringSlow(pArena, scan, bound);
        }
    }
    String result = CopyCStringRange(start, scan.pCurrent, pArena);
    scan.pCurrent++;
    return result;
}

// ***********************************************************************

f64 Scan::ParseNumber(Scan::ScanningState& scan) {
    scan.pCurrent -= 1;  // Go back to get the first digit or symbol
    byte* start = scan.pCurrent;

    // Hex number
    if (Scan::Peek(scan) == '0' && (Scan::PeekNext(scan) == 'x' || Scan::PeekNext(scan) == 'X')) {
        Scan::Advance(scan);
        Scan::Advance(scan);
        while (Scan::IsHexDigit(Scan::Peek(scan))) {
            Scan::Advance(scan);
        }
    }
    // Normal number
    else {
        byte c = Scan::Peek(scan);
        while (Scan::IsDigit(c) || c == '+' || c == '-' || c == '.' || c == 'E' || c == 'e') {
            Scan::Advance(scan);
            c = Scan::Peek(scan);
        }
    }

    // TODO: error report. This returns 0.0 if no conversion possible. We can look at the literal
    // string and see If it's 0.0, ".0", "0." or 0. if not there's been an error in the parsing. I
    // know this is cheeky. I don't care.
    return strtod(start, &scan.pCurrent);
}
