// Copyright 2020-2022 David Colson. All rights reserved.
#pragma once

struct JsonValue {
    enum class Type {
        Object,
        Array,
        Floating,
        Integer,
        Boolean,
        String,
        Null
    };

    JsonValue();

    static JsonValue NewObject();
    static JsonValue NewArray();

    bool IsNull() const;
    bool IsArray() const;
    bool IsObject() const;
    bool HasKey(String identifier);
    int Count() const;

    String ToString() const;
    f64 ToFloat() const;
    long ToInt() const;
    bool ToBool() const;

    JsonValue& operator[](String identifier);
    JsonValue& operator[](u64 index);

    JsonValue& Get(String identifier);
    JsonValue& Get(u64 index);

    void Append(JsonValue& value);

    Type type;
    Arena* pArena { nullptr };
    union {
        f64 floatNumber { 0.0f };
        HashMap<String, JsonValue> object;
        ResizableArray<JsonValue> array;
        String string;
        long intNumber;
        bool boolean;
    };
};

JsonValue ParseJsonFile(Arena* pArena, String file);
String SerializeJsonValue(JsonValue json);
