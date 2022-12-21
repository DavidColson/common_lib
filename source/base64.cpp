// Copyright 2020-2022 David Colson. All rights reserved.

#include "base64.h"

static const unsigned char encodingTable[65] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// ***********************************************************************

String DecodeBase64(String const& encodedString, IAllocator* pAlloc) {
    unsigned char decodingTable[256];

    for (size_t i = 0; i < sizeof(encodingTable) - 1; i++)
        decodingTable[encodingTable[i]] = (unsigned char)i;
    decodingTable['='] = 0;

    const char* input = encodedString.m_pData;
    size_t inputLength = encodedString.m_length;

    size_t count = 0;
    for (int i = 0; i < inputLength; i++) {
        if (decodingTable[input[i]] != 0x80)
            count++;
    }

    if (count == 0 || count % 4) {
        // Log::Crit("Invalid base64 encoded string");
        return String();
    }

    size_t outputlen = count / 4 * 3;


    char* output = (char*)pAlloc->Allocate(outputlen * sizeof(char));
    char* position = output;

    for (int i = 0; i < inputLength; i += 4) {
        char a = decodingTable[input[i]] & 0xFF;
        char b = decodingTable[input[i + 1]] & 0xFF;
        char c = decodingTable[input[i + 2]] & 0xFF;
        char d = decodingTable[input[i + 3]] & 0xFF;

        *position++ = a << 2 | (b & 0x30) >> 4;
        if (c != 0x40)
            *position++ = b << 4 | (c & 0x3c) >> 2;
        if (d != 0x40)
            *position++ = (c << 6 | d);
    }

    String result;
    result.m_pData = output;
    result.m_length = outputlen;
    return result;
}

// ***********************************************************************

String EncodeBase64(size_t length, const char* bytes, IAllocator* pAlloc) {
    size_t outputLength = length * 4 / 3 + 4;  // 3-byte blocks to 4-byte
    outputLength++;                            // nul termination
    if (outputLength < length)
        return String();  // integer overflow
    char* output = (char*)pAlloc->Allocate(outputLength * sizeof(char));
    char* position = output;

    for (int i = 0; i < length; i += 3) {
        int nChars = (int)length - i;
        char a = bytes[i];
        char b = nChars == 1 ? 0xF : bytes[i + 1];
        char c = nChars < 3 ? 0x3F : bytes[i + 2];

        *position++ = encodingTable[a >> 2];
        *position++ = encodingTable[(a & 0x3) << 4 | b >> 4];
        *position++ = encodingTable[b == 0xF ? 0x40 : (b & 0xF) << 2 | c >> 6];
        *position++ = encodingTable[c == 0x3F ? 0x40 : (c & 0x3F)];
    }

    String result;
    result.m_pData = output;
    result.m_length = outputLength;
    return result;
}
