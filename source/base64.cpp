// Copyright 2020-2022 David Colson. All rights reserved.

#include "base64.h"

#include "light_string.h"

static const byte encodingTable[65] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// ***********************************************************************

String DecodeBase64(String const& encodedString, IAllocator* pAlloc) {
    byte decodingTable[256];

    for (usize i = 0; i < sizeof(encodingTable) - 1; i++)
        decodingTable[encodingTable[i]] = (byte)i;
    decodingTable['='] = 0;

    const byte* input = encodedString.pData;
    usize inputLength = encodedString.length;

    usize count = 0;
    for (int i = 0; i < inputLength; i++) {
        if (decodingTable[input[i]] != 0x80)
            count++;
    }

    if (count == 0 || count % 4) {
        // Log::Crit("Invalid base64 encoded string");
        return String();
    }

    usize outputlen = count / 4 * 3;


    byte* output = (byte*)pAlloc->Allocate(outputlen * sizeof(byte));
    byte* position = output;

    for (int i = 0; i < inputLength; i += 4) {
        byte a = decodingTable[input[i]] & 0xFF;
        byte b = decodingTable[input[i + 1]] & 0xFF;
        byte c = decodingTable[input[i + 2]] & 0xFF;
        byte d = decodingTable[input[i + 3]] & 0xFF;

        *position++ = a << 2 | (b & 0x30) >> 4;
        if (c != 0x40)
            *position++ = b << 4 | (c & 0x3c) >> 2;
        if (d != 0x40)
            *position++ = (c << 6 | d);
    }

    String result;
    result.pData = output;
    result.length = outputlen;
    return result;
}

// ***********************************************************************

String EncodeBase64(usize length, const byte* bytes, IAllocator* pAlloc) {
    usize outputLength = length * 4 / 3 + 4;  // 3-byte blocks to 4-byte
    outputLength++;                            // nul termination
    if (outputLength < length)
        return String();  // integer overflow
    byte* output = (byte*)pAlloc->Allocate(outputLength * sizeof(byte));
    byte* position = output;

    for (int i = 0; i < length; i += 3) {
        int nbytes = (int)length - i;
        byte a = bytes[i];
        byte b = nbytes == 1 ? 0xF : bytes[i + 1];
        byte c = nbytes < 3 ? 0x3F : bytes[i + 2];

        *position++ = encodingTable[a >> 2];
        *position++ = encodingTable[(a & 0x3) << 4 | b >> 4];
        *position++ = encodingTable[b == 0xF ? 0x40 : (b & 0xF) << 2 | c >> 6];
        *position++ = encodingTable[c == 0x3F ? 0x40 : (c & 0x3F)];
    }

    String result;
    result.pData = output;
    result.length = outputLength;
    return result;
}
