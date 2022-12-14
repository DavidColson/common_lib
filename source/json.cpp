// Copyright 2020-2022 David Colson. All rights reserved.

#include "json.h"

#include "scanning.h"
#include "string_builder.h"
#include "hashmap.inl"
#include "resizable_array.inl"

#include <math.h>

#define ASSERT(condition, text)

// Tokenizer
///////////////////////////

enum class TokenType {
    // Single characters
    LeftBracket,
    RightBracket,
    LeftBrace,
    RightBrace,
    Comma,
    Colon,

    // Identifiers and keywords
    Boolean,
    Null,
    Identifier,

    // Everything else
    Number,
    String
};

struct Token {
    Token(TokenType type) : m_type(type) {}

    Token(TokenType type, String _stringOrIdentifier)
        : m_type(type), m_stringOrIdentifier(_stringOrIdentifier) {}

    Token(TokenType type, double _number) : m_type(type), m_number(_number) {}

    Token(TokenType type, bool _boolean) : m_type(type), m_boolean(_boolean) {}

    TokenType m_type;
    String m_stringOrIdentifier;
    double m_number;
    bool m_boolean;
};

// ***********************************************************************

String ParseStringSlow(IAllocator* pAllocator, Scan::ScanningState& scan, char bound) {
    char* start = scan.m_pCurrent;
    char* pos = start;
    while (*pos != bound && !Scan::IsAtEnd(scan)) {
        pos++;
    }
    size_t count = pos - start;
    char* outputString = new char[count * 2];  // to allow for escape chars TODO: Convert to Mallloc
    pos = outputString;

    char* cursor = scan.m_pCurrent;

    for (size_t i = 0; i < count; i++) {
        char c = *(cursor++);

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
            char next = *(cursor++);
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

    scan.m_pCurrent = cursor;

    String result = CopyCStringRange(outputString, pos, pAllocator);
    delete outputString;
    return result;
}

// ***********************************************************************

String ParseString(IAllocator* pAllocator, Scan::ScanningState& scan, char bound) {
    char* start = scan.m_pCurrent;
    while (*(scan.m_pCurrent) != bound && !Scan::IsAtEnd(scan)) {
        if (*(scan.m_pCurrent++) == '\\') {
            scan.m_pCurrent = start;
            return ParseStringSlow(pAllocator, scan, bound);
        }
    }
    String result = CopyCStringRange(start, scan.m_pCurrent, pAllocator);
    scan.m_pCurrent++;
    return result;
}

// ***********************************************************************

double ParseNumber(Scan::ScanningState& scan) {
    scan.m_pCurrent -= 1;  // Go back to get the first digit or symbol
    char* start = scan.m_pCurrent;

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
        char c = Scan::Peek(scan);
        while (Scan::IsDigit(c) || c == '+' || c == '-' || c == '.' || c == 'E' || c == 'e') {
            Scan::Advance(scan);
            c = Scan::Peek(scan);
        }
    }

    // TODO: error report. This returns 0.0 if no conversion possible. We can look at the literal
    // string and see If it's 0.0, ".0", "0." or 0. if not there's been an error in the parsing. I
    // know this is cheeky. I don't care.
    return strtod(start, &scan.m_pCurrent);
}

// ***********************************************************************

ResizableArray<Token> TokenizeJson(IAllocator* pAllocator, String jsonText) {
    Scan::ScanningState scan;
    scan.m_pTextStart = jsonText.m_pData;
    scan.m_pTextEnd = jsonText.m_pData + jsonText.m_length;
    scan.m_pCurrent = (char*)scan.m_pTextStart;
    scan.m_line = 1;

    ResizableArray<Token> tokens(pAllocator);

    while (!Scan::IsAtEnd(scan)) {
        char c = Scan::Advance(scan);
        int column = int(scan.m_pCurrent - scan.m_pCurrentLineStart);
        char* loc = scan.m_pCurrent - 1;
        switch (c) {
            // Single character tokens
            case '[': tokens.PushBack(Token { TokenType::LeftBracket }); break;
            case ']': tokens.PushBack(Token { TokenType::RightBracket }); break;
            case '{': tokens.PushBack(Token { TokenType::LeftBrace }); break;
            case '}': tokens.PushBack(Token { TokenType::RightBrace }); break;
            case ':': tokens.PushBack(Token { TokenType::Colon }); break;
            case ',': tokens.PushBack(Token { TokenType::Comma }); break;

            // Comments!
            case '/':
                if (Scan::Match(scan, '/')) {
                    while (Scan::Peek(scan) != '\n')
                        Scan::Advance(scan);
                } else if (Scan::Match(scan, '*')) {
                    while (!(Scan::Peek(scan) == '*' && Scan::PeekNext(scan) == '/'))
                        Scan::Advance(scan);

                    Scan::Advance(scan);  // *
                    Scan::Advance(scan);  // /
                }
                break;

            // Whitespace
            case ' ':
            case '\r':
            case '\t': break;
            case '\n':
                scan.m_line++;
                scan.m_pCurrentLineStart = scan.m_pCurrent;
                break;

            // String literals
            case '\'': {
                String string = ParseString(pAllocator, scan, '\'');
                tokens.PushBack(Token { TokenType::String, string });
                break;
            }
            case '"': {
                String string = ParseString(pAllocator, scan, '"');
                tokens.PushBack(Token { TokenType::String, string });
                break;
            }

            default:
                // Numbers
                if (Scan::IsDigit(c) || c == '+' || c == '-' || c == '.') {
                    double num = ParseNumber(scan);
                    tokens.PushBack(Token { TokenType::Number, num });
                    break;
                }

                // Identifiers and keywords
                if (Scan::IsAlpha(c)) {
                    while (Scan::IsAlphaNumeric(Scan::Peek(scan)))
                        Scan::Advance(scan);

                    String identifier;
                    identifier.m_pData = loc;
                    identifier.m_length = scan.m_pCurrent - loc;

                    // Check for keywords
                    if (identifier == "true")
                        tokens.PushBack(Token { TokenType::Boolean, true });
                    else if (identifier == "false")
                        tokens.PushBack(Token { TokenType::Boolean, false });
                    else if (identifier == "null")
                        tokens.PushBack(Token { TokenType::Null });
                    else {
                        identifier = CopyCStringRange(loc, scan.m_pCurrent, pAllocator);
                        tokens.PushBack(Token { TokenType::Identifier, identifier });
                    }
                }
                break;
        }
    }
    return tokens;
}


// ***********************************************************************

HashMap<String, JsonValue> ParseObject(
    IAllocator* pAllocator, ResizableArray<Token>& tokens, int& currentToken);
ResizableArray<JsonValue> ParseArray(
    IAllocator* pAllocator, ResizableArray<Token>& tokens, int& currentToken);

JsonValue ParseValue(IAllocator* pAllocator, ResizableArray<Token>& tokens, int& currentToken) {
    Token& token = tokens[currentToken];

    switch (token.m_type) {
        case TokenType::LeftBrace: {
            JsonValue v;
            v.m_pAllocator = pAllocator;
            v.m_object = ParseObject(pAllocator, tokens, currentToken);
            v.m_type = JsonValue::Type::Object;
            return v;
            break;
        }
        case TokenType::LeftBracket: {
            JsonValue v;
            v.m_pAllocator = pAllocator;
            v.m_array = ParseArray(pAllocator, tokens, currentToken);
            v.m_type = JsonValue::Type::Array;
            return v;
            break;
        }
        case TokenType::String: {
            currentToken++;
            JsonValue v;
            v.m_pAllocator = pAllocator;
            v.m_string =
                token.m_stringOrIdentifier;  // TODO: Potential copy required if tokens are freed
            v.m_type = JsonValue::Type::String;
            return v;
            break;
        }
        case TokenType::Number: {
            currentToken++;
            JsonValue v;
            double n = token.m_number;
            double intPart;
            if (modf(n, &intPart) == 0.0) {
                v.m_intNumber = (long)intPart;
                v.m_type = JsonValue::Type::Integer;
            } else {
                v.m_floatNumber = n;
                v.m_type = JsonValue::Type::Floating;
            }

            return v;
            break;
        }
        case TokenType::Boolean: {
            currentToken++;
            JsonValue v;
            v.m_boolean = token.m_boolean;
            v.m_type = JsonValue::Type::Boolean;
            return v;
            break;
        }
        case TokenType::Null: currentToken++;
        default: return JsonValue(); break;
    }
    return JsonValue();
}

// ***********************************************************************

HashMap<String, JsonValue> ParseObject(
    IAllocator* pAllocator, ResizableArray<Token>& tokens, int& currentToken) {
    currentToken++;  // Advance over opening brace

    // TODO: This should construct an empty jsonValue as an object and fill the map in in place, so
    // there is no need to deep copy the map
    HashMap<String, JsonValue> map(pAllocator);
    while (currentToken < (int)tokens.m_count
           && tokens[currentToken].m_type != TokenType::RightBrace) {
        // We expect,
        // identifier or string
        if (tokens[currentToken].m_type != TokenType::Identifier
            && tokens[currentToken].m_type != TokenType::String) {}
        // Log::Crit("Expected identifier or string");

        String key = tokens[currentToken].m_stringOrIdentifier;
        currentToken += 1;

        // colon
        if (tokens[currentToken].m_type != TokenType::Colon) {}
        // Log::Crit("Expected colon");
        currentToken += 1;

        // String, Number, Boolean, Null
        // If left bracket or brace encountered, skip until closing
        map[key] = ParseValue(pAllocator, tokens, currentToken);

        // Comma, or right brace
        if (tokens[currentToken].m_type == TokenType::RightBrace)
            break;
        if (tokens[currentToken].m_type != TokenType::Comma) {}
        // Log::Crit("Expected comma or Right Curly Brace");
        currentToken += 1;
    }
    currentToken++;  // Advance over closing brace
    return map;
}

// ***********************************************************************

ResizableArray<JsonValue> ParseArray(
    IAllocator* pAllocator, ResizableArray<Token>& tokens, int& currentToken) {
    currentToken++;  // Advance over opening bracket

    ResizableArray<JsonValue> array(pAllocator);
    while (currentToken < (int)tokens.m_count
           && tokens[currentToken].m_type != TokenType::RightBracket) {
        // We expect,
        // String, Number, Boolean, Null
        array.PushBack(ParseValue(pAllocator, tokens, currentToken));

        // Comma, or right brace
        if (tokens[currentToken].m_type == TokenType::RightBracket)
            break;
        if (tokens[currentToken].m_type != TokenType::Comma) {}
        // Log::Crit("Expected comma or right bracket");
        currentToken += 1;
    }
    currentToken++;  // Advance over closing bracket
    return array;
}


// JsonValue implementation
///////////////////////////

// ***********************************************************************

JsonValue::JsonValue() {
    m_type = Type::Null;
}

// ***********************************************************************

void JsonValue::Free() {
    if (m_type == Type::Object) {
        m_object.Free([this](HashNode<String, JsonValue>& node) {
            FreeString(node.key, m_pAllocator);
            node.value.Free();
        });
    }
    if (m_type == Type::Array) {
        m_array.Free([](JsonValue& value) { value.Free(); });
    }
    if (m_type == Type::String) {
        FreeString(m_string, m_pAllocator);
    }
}

// ***********************************************************************

String JsonValue::ToString() const {
    if (m_type == Type::String)
        return m_string;
    return String();
}

// ***********************************************************************

double JsonValue::ToFloat() const {
    if (m_type == Type::Floating)
        return m_floatNumber;
    else if (m_type == Type::Integer)
        return (double)m_intNumber;
    return 0.0f;
}

// ***********************************************************************

long JsonValue::ToInt() const {
    if (m_type == Type::Integer)
        return m_intNumber;
    return 0;
}

// ***********************************************************************

bool JsonValue::ToBool() const {
    if (m_type == Type::Boolean)
        return m_boolean;
    return 0;
}

// ***********************************************************************

bool JsonValue::IsNull() const {
    return m_type == Type::Null;
}

// ***********************************************************************

bool JsonValue::IsArray() const {
    return m_type == Type::Array;
}

// ***********************************************************************

bool JsonValue::IsObject() const {
    return m_type == Type::Object;
}

// ***********************************************************************

bool JsonValue::HasKey(String identifier) {
    ASSERT(m_type == Type::Object, "Attempting to treat this value as an object when it is not.");
    return m_object.Get(identifier) != nullptr;
}

// ***********************************************************************

int JsonValue::Count() const {
    ASSERT(
        m_type == Type::Array || m_type == Type::Object,
        "Attempting to treat this value as an array or object when it is not.");
    if (m_type == Type::Array)
        return (int)m_array.m_count;
    else
        return (int)m_object.m_count;
}

// ***********************************************************************

JsonValue& JsonValue::operator[](String identifier) {
    ASSERT(m_type == Type::Object, "Attempting to treat this value as an object when it is not.");
    return m_object[identifier];
}

// ***********************************************************************

JsonValue& JsonValue::operator[](size_t index) {
    ASSERT(m_type == Type::Array, "Attempting to treat this value as an array when it is not.");
    ASSERT(
        m_array.count > index,
        "Accessing an element that does not exist in this array, you probably need to append");
    return m_array[index];
}

// ***********************************************************************

JsonValue& JsonValue::Get(String identifier) {
    ASSERT(m_type == Type::Object, "Attempting to treat this value as an object when it is not.");
    return m_object[identifier];
}

// ***********************************************************************

JsonValue& JsonValue::Get(size_t index) {
    ASSERT(m_type == Type::Array, "Attempting to treat this value as an array when it is not.");
    ASSERT(
        m_array.count > index,
        "Accessing an element that does not exist in this array, you probably need to append");
    return m_array[index];
}

// ***********************************************************************

void JsonValue::Append(JsonValue& value) {
    ASSERT(m_type == Type::Array, "Attempting to treat this value as an array when it is not.");
    m_array.PushBack(value);
}

// ***********************************************************************

JsonValue JsonValue::NewObject() {
    JsonValue v;
    v.m_type = Type::Object;
    return v;
}

// ***********************************************************************

JsonValue JsonValue::NewArray() {
    JsonValue v;
    v.m_type = Type::Array;
    return v;
}

// ***********************************************************************

JsonValue ParseJsonFile(String file, IAllocator* pAllocator) {
    ResizableArray<Token> tokens = TokenizeJson(pAllocator, file);

    int firstToken = 0;
    JsonValue json5 = ParseValue(pAllocator, tokens, firstToken);
    tokens.Free();
    return json5;
}

// ***********************************************************************

void SerializeJsonInternal(JsonValue json, StringBuilder& builder, int indentCount) {
    auto printIndentation = [&builder](int level) {
        for (int j = 0; j < level; j++) {
            builder.Append("    ");
        }
    };

    switch (json.m_type) {
        case JsonValue::Type::Array:
            builder.Append("[");
            if (json.Count() > 0)
                builder.Append("\n");

            for (const JsonValue& val : json.m_array) {
                printIndentation(indentCount + 1);
                SerializeJsonInternal(val, builder, indentCount + 1);
                builder.Append(", \n");
            }
            printIndentation(indentCount);
            builder.Append("]");
            break;
        case JsonValue::Type::Object: {
            builder.Append("{");
            if (json.Count() > 0)
                builder.Append("\n");

            for (size_t i = 0; i < json.m_object.m_tableSize; i++) {
                HashNode<String, JsonValue>& node = json.m_object.m_pTable[i];
                if (node.hash != UNUSED_HASH) {
                    printIndentation(indentCount + 1);
                    builder.AppendFormat("\"%s\" : ", node.key.m_pData);
                    SerializeJsonInternal(node.value, builder, indentCount + 1);
                    builder.Append(", \n");
                }
            }

            printIndentation(indentCount);
            builder.Append("}");
        } break;
        case JsonValue::Type::Floating: builder.AppendFormat("%.17g", json.ToFloat()); break;
        // TODO: Serialize with exponentials like we do with floats
        case JsonValue::Type::Integer: builder.AppendFormat("%i", json.ToInt()); break;
        case JsonValue::Type::Boolean:
            builder.AppendFormat("%s", json.ToBool() ? "true" : "false");
            break;
        case JsonValue::Type::String: builder.AppendFormat("\"%s\"", json.ToString().m_pData); break;
        case JsonValue::Type::Null: builder.Append("null"); break;
        default: builder.Append("CANT SERIALIZE YET"); break;
    }

    // Kinda hacky thing that makes small jsonValues serialize as collapsed, easier to read files
    // imo

    // Replace newlines with nothing

    // if (result.size() < 100)
    //{
    //	size_t index = 0;
    //	while (true) {
    //		index = result.find("\n", index);
    //		int count = 1;
    //		while((index+count) != size_t(-1) && result[index+count] == ' ')
    //			count++;
    //		if (index == size_t(-1)) break;
    //		result.replace(index, count, "");
    //		index += 1;
    //	}
    // }
}

// ***********************************************************************

String SerializeJsonValue(IAllocator* pAllocator, JsonValue json) {
    StringBuilder builder(pAllocator);
    SerializeJsonInternal(json, builder, 0);
    return builder.CreateString(true, pAllocator);
}
