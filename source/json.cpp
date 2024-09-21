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
    Token(TokenType type) : type(type) {}

    Token(TokenType type, String _stringOrIdentifier)
        : type(type), stringOrIdentifier(_stringOrIdentifier) {}

    Token(TokenType type, f64 _number) : type(type), number(_number) {}

    Token(TokenType type, bool _boolean) : type(type), boolean(_boolean) {}

    TokenType type;
    String stringOrIdentifier;
    f64 number;
    bool boolean;
};

// ***********************************************************************

ResizableArray<Token> TokenizeJson(Arena* pArena, String jsonText) {
    Scan::ScanningState scan;
    scan.pTextStart = jsonText.pData;
    scan.pTextEnd = jsonText.pData + jsonText.length;
    scan.pCurrent = (byte*)scan.pTextStart;
    scan.line = 1;

    ResizableArray<Token> tokens(pArena);

    while (!Scan::IsAtEnd(scan)) {
        byte c = Scan::Advance(scan);
        int column = int(scan.pCurrent - scan.pCurrentLineStart);
        byte* loc = scan.pCurrent - 1;
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
                scan.line++;
                scan.pCurrentLineStart = scan.pCurrent;
                break;

            // String literals
            case '\'': {
                String string = ParseString(pArena, scan, '\'');
                tokens.PushBack(Token { TokenType::String, string });
                break;
            }
            case '"': {
                String string = ParseString(pArena, scan, '"');
                tokens.PushBack(Token { TokenType::String, string });
                break;
            }

            default:
                // Numbers
                if (Scan::IsDigit(c) || c == '+' || c == '-' || c == '.') {
                    f64 num = ParseNumber(scan);
                    tokens.PushBack(Token { TokenType::Number, num });
                    break;
                }

                // Identifiers and keywords
                if (Scan::IsAlpha(c)) {
                    while (Scan::IsAlphaNumeric(Scan::Peek(scan)))
                        Scan::Advance(scan);

                    String identifier;
                    identifier.pData = loc;
                    identifier.length = scan.pCurrent - loc;

                    // Check for keywords
                    if (identifier == "true")
                        tokens.PushBack(Token { TokenType::Boolean, true });
                    else if (identifier == "false")
                        tokens.PushBack(Token { TokenType::Boolean, false });
                    else if (identifier == "null")
                        tokens.PushBack(Token { TokenType::Null });
                    else {
                        identifier = CopyCStringRange(loc, scan.pCurrent, pArena);
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
    Arena* pArena, ResizableArray<Token>& tokens, int& currentToken);
ResizableArray<JsonValue> ParseArray(
    Arena* pArena, ResizableArray<Token>& tokens, int& currentToken);

JsonValue ParseValue(Arena* pArena, ResizableArray<Token>& tokens, int& currentToken) {
    Token& token = tokens[currentToken];

    switch (token.type) {
        case TokenType::LeftBrace: {
            JsonValue v;
            v.pArena = pArena;
            v.object = ParseObject(pArena, tokens, currentToken);
            v.type = JsonValue::Type::Object;
            return v;
            break;
        }
        case TokenType::LeftBracket: {
            JsonValue v;
            v.pArena = pArena;
            v.array = ParseArray(pArena, tokens, currentToken);
            v.type = JsonValue::Type::Array;
            return v;
            break;
        }
        case TokenType::String: {
            currentToken++;
            JsonValue v;
            v.pArena = pArena;
            v.string = token.stringOrIdentifier;  // TODO: Potential copy required if tokens are freed
            v.type = JsonValue::Type::String;
            return v;
            break;
        }
        case TokenType::Number: {
            currentToken++;
            JsonValue v;
            f64 n = token.number;
            f64 intPart;
            if (modf(n, &intPart) == 0.0) {
                v.intNumber = (long)intPart;
                v.type = JsonValue::Type::Integer;
            } else {
                v.floatNumber = n;
                v.type = JsonValue::Type::Floating;
            }

            return v;
            break;
        }
        case TokenType::Boolean: {
            currentToken++;
            JsonValue v;
            v.boolean = token.boolean;
            v.type = JsonValue::Type::Boolean;
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
    Arena* pArena, ResizableArray<Token>& tokens, int& currentToken) {
    currentToken++;  // Advance over opening brace

    // TODO: This should construct an empty jsonValue as an object and fill the map in in place, so
    // there is no need to deep copy the map
    HashMap<String, JsonValue> map(pArena);
    while (currentToken < (int)tokens.count
           && tokens[currentToken].type != TokenType::RightBrace) {
        // We expect,
        // identifier or string
        if (tokens[currentToken].type != TokenType::Identifier
            && tokens[currentToken].type != TokenType::String) {}
        // Log::Crit("Expected identifier or string");

        String key = tokens[currentToken].stringOrIdentifier;
        currentToken += 1;

        // colon
        if (tokens[currentToken].type != TokenType::Colon) {}
        // Log::Crit("Expected colon");
        currentToken += 1;

        // String, Number, Boolean, Null
        // If left bracket or brace encountered, skip until closing
        map[key] = ParseValue(pArena, tokens, currentToken);

        // Comma, or right brace
        if (tokens[currentToken].type == TokenType::RightBrace)
            break;
        if (tokens[currentToken].type != TokenType::Comma) {}
        // Log::Crit("Expected comma or Right Curly Brace");
        currentToken += 1;
    }
    currentToken++;  // Advance over closing brace
    return map;
}

// ***********************************************************************

ResizableArray<JsonValue> ParseArray(
    Arena* pArena, ResizableArray<Token>& tokens, int& currentToken) {
    currentToken++;  // Advance over opening bracket

    ResizableArray<JsonValue> array(pArena);
    while (currentToken < (int)tokens.count
           && tokens[currentToken].type != TokenType::RightBracket) {
        // We expect,
        // String, Number, Boolean, Null
        array.PushBack(ParseValue(pArena, tokens, currentToken));

        // Comma, or right brace
        if (tokens[currentToken].type == TokenType::RightBracket)
            break;
        if (tokens[currentToken].type != TokenType::Comma) {}
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
    type = Type::Null;
}

// ***********************************************************************

String JsonValue::ToString() const {
    if (type == Type::String)
        return string;
    return String();
}

// ***********************************************************************

f64 JsonValue::ToFloat() const {
    if (type == Type::Floating)
        return floatNumber;
    else if (type == Type::Integer)
        return (f64)intNumber;
    return 0.0f;
}

// ***********************************************************************

long JsonValue::ToInt() const {
    if (type == Type::Integer)
        return intNumber;
    return 0;
}

// ***********************************************************************

bool JsonValue::ToBool() const {
    if (type == Type::Boolean)
        return boolean;
    return 0;
}

// ***********************************************************************

bool JsonValue::IsNull() const {
    return type == Type::Null;
}

// ***********************************************************************

bool JsonValue::IsArray() const {
    return type == Type::Array;
}

// ***********************************************************************

bool JsonValue::IsObject() const {
    return type == Type::Object;
}

// ***********************************************************************

bool JsonValue::HasKey(String identifier) {
    ASSERT(type == Type::Object, "Attempting to treat this value as an object when it is not.");
    return object.Get(identifier) != nullptr;
}

// ***********************************************************************

int JsonValue::Count() const {
    ASSERT(
        type == Type::Array || type == Type::Object,
        "Attempting to treat this value as an array or object when it is not.");
    if (type == Type::Array)
        return (int)array.count;
    else
        return (int)object.count;
}

// ***********************************************************************

JsonValue& JsonValue::operator[](String identifier) {
    ASSERT(type == Type::Object, "Attempting to treat this value as an object when it is not.");
    return object[identifier];
}

// ***********************************************************************

JsonValue& JsonValue::operator[](usize index) {
    ASSERT(type == Type::Array, "Attempting to treat this value as an array when it is not.");
    ASSERT(
        array.count > index,
        "Accessing an element that does not exist in this array, you probably need to append");
    return array[index];
}

// ***********************************************************************

JsonValue& JsonValue::Get(String identifier) {
    ASSERT(type == Type::Object, "Attempting to treat this value as an object when it is not.");
    return object[identifier];
}

// ***********************************************************************

JsonValue& JsonValue::Get(usize index) {
    ASSERT(type == Type::Array, "Attempting to treat this value as an array when it is not.");
    ASSERT(
        array.count > index,
        "Accessing an element that does not exist in this array, you probably need to append");
    return array[index];
}

// ***********************************************************************

void JsonValue::Append(JsonValue& value) {
    ASSERT(type == Type::Array, "Attempting to treat this value as an array when it is not.");
    array.PushBack(value);
}

// ***********************************************************************

JsonValue JsonValue::NewObject() {
    JsonValue v;
    v.type = Type::Object;
    return v;
}

// ***********************************************************************

JsonValue JsonValue::NewArray() {
    JsonValue v;
    v.type = Type::Array;
    return v;
}

// ***********************************************************************

JsonValue ParseJsonFile(Arena* pArena, String file) {
    ResizableArray<Token> tokens = TokenizeJson(pArena, file);

    int firstToken = 0;
    JsonValue json5 = ParseValue(pArena, tokens, firstToken);
    return json5;
}

// ***********************************************************************

void SerializeJsonInternal(JsonValue json, StringBuilder& builder, int indentCount) {
    auto printIndentation = [&builder](int level) {
        for (int j = 0; j < level; j++) {
            builder.Append("    ");
        }
    };

    switch (json.type) {
        case JsonValue::Type::Array:
            builder.Append("[");
            if (json.Count() > 0)
                builder.Append("\n");

            for (const JsonValue& val : json.array) {
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

            for (size i = 0; i < json.object.tableSize; i++) {
                HashNode<String, JsonValue>& node = json.object.pTable[i];
                if (node.hash != UNUSED_HASH) {
                    printIndentation(indentCount + 1);
                    builder.AppendFormat("\"%s\" : ", node.key.pData);
                    SerializeJsonInternal(node.value, builder, indentCount + 1);
                    builder.Append(", \n");
                }
            }

            printIndentation(indentCount);
            builder.Append("}");
        } break;
        case JsonValue::Type::Floating: builder.AppendFormat("%.17g", json.ToFloat()); break;
        // TODO: Serialize with exponentials like we do with f32s
        case JsonValue::Type::Integer: builder.AppendFormat("%i", json.ToInt()); break;
        case JsonValue::Type::Boolean:
            builder.AppendFormat("%s", json.ToBool() ? "true" : "false");
            break;
        case JsonValue::Type::String: builder.AppendFormat("\"%s\"", json.ToString().pData); break;
        case JsonValue::Type::Null: builder.Append("null"); break;
        default: builder.Append("CANT SERIALIZE YET"); break;
    }

    // Kinda hacky thing that makes small jsonValues serialize as collapsed, easier to read files
    // imo

    // Replace newlines with nothing

    // if (result.size() < 100)
    //{
    //	usize index = 0;
    //	while (true) {
    //		index = result.find("\n", index);
    //		int count = 1;
    //		while((index+count) != usize(-1) && result[index+count] == ' ')
    //			count++;
    //		if (index == usize(-1)) break;
    //		result.replace(index, count, "");
    //		index += 1;
    //	}
    // }
}

// ***********************************************************************

String SerializeJsonValue(Arena* pArena, JsonValue json) {
    StringBuilder builder(pArena);
    SerializeJsonInternal(json, builder, 0);
    return builder.CreateString(pArena, true);
}
