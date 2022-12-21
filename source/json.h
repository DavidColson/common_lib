// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

#include "hashmap.h"
#include "light_string.h"
#include "resizable_array.h"

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
    double ToFloat() const;
    long ToInt() const;
    bool ToBool() const;

    JsonValue& operator[](String identifier);
    JsonValue& operator[](size_t index);

    JsonValue& Get(String identifier);
    JsonValue& Get(size_t index);

    void Append(JsonValue& value);

    void Free();

    Type m_type;
    IAllocator* m_pAllocator { nullptr };
    union {
        double m_floatNumber { 0.0f };
        HashMap<String, JsonValue> m_object;
        ResizableArray<JsonValue> m_array;
        String m_string;
        long m_intNumber;
        bool m_boolean;
    };
};

JsonValue ParseJsonFile(String file, IAllocator* pAllocator = &g_Allocator);
String SerializeJsonValue(JsonValue json);
