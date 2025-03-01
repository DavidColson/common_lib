// Copyright 2020-2022 David Colson. All rights reserved.

static const u8 encodingTable[65] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static i32 modTable[] = { 0, 2, 1 };

// ***********************************************************************

String DecodeBase64(Arena* pArena, String const& encodedString) {
    u8 decodingTable[256];

    for (u64 i = 0; i < sizeof(encodingTable) - 1; i++)
        decodingTable[encodingTable[i]] = (u8)i;
    decodingTable['='] = 0;

    const u8* input = (u8*)encodedString.pData;
    u64 inputLength = encodedString.length;

    u64 count = 0;
    for (int i = 0; i < inputLength; i++) {
        if (decodingTable[input[i]] != 0x80)
            count++;
    }

    if (count == 0 || count % 4) {
        // Log::Crit("Invalid base64 encoded string");
        return String();
    }

    u64 outputlen = count / 4 * 3;

    u8* output = New(pArena, u8, outputlen);
    u8* position = output;

    for (int i = 0; i < inputLength; i += 4) {
        u8 a = decodingTable[input[i]] & 0xFF;
        u8 b = decodingTable[input[i + 1]] & 0xFF;
        u8 c = decodingTable[input[i + 2]] & 0xFF;
        u8 d = decodingTable[input[i + 3]] & 0xFF;

        *position++ = a << 2 | (b & 0x30) >> 4;
        if (c != 0x40)
            *position++ = b << 4 | (c & 0x3c) >> 2;
        if (d != 0x40)
            *position++ = (c << 6 | d);
    }

	if (input[inputLength - 1] == '=') outputlen--;
    if (input[inputLength - 2] == '=') outputlen--;

    String result;
    result.pData = (char*)output;
    result.length = outputlen;
    return result;
}

// ***********************************************************************

String EncodeBase64(Arena* pArena, u64 length, const u8* bytes) {
    u64 outputLength = 4 * ((length + 2) / 3);  // 3-u8 blocks to 4-u8

    if (outputLength < length)
        return String();  // integer overflow
    u8* output = New(pArena, u8, outputLength);

    for (int i = 0, j = 0; i < length;) {
        uint32_t octet_a = i < length ? (unsigned char)bytes[i++] : 0;
        uint32_t octet_b = i < length ? (unsigned char)bytes[i++] : 0;
        uint32_t octet_c = i < length ? (unsigned char)bytes[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        output[j++] = encodingTable[(triple >> 3 * 6) & 0x3F];
        output[j++] = encodingTable[(triple >> 2 * 6) & 0x3F];
        output[j++] = encodingTable[(triple >> 1 * 6) & 0x3F];
        output[j++] = encodingTable[(triple >> 0 * 6) & 0x3F];
    }

    for (int i = 0; i < modTable[length % 3]; i++)
        output[outputLength - 1 - i] = '=';

    String result;
    result.pData = (char*)output;
    result.length = outputLength;
    return result;
}
