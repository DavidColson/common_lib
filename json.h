// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

#include "resizable_array.h"
#include "hashmap.h"
#include "string.h"

struct JsonValue
{
	enum class Type
	{
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
	union Data
	{
		ResizableArray<JsonValue> m_pArray{};
		HashMap<String, JsonValue> m_pObject;
		String m_pString;
		double m_floatingNumber;
		long m_integerNumber;
		bool m_boolean;
	} m_internalData;
};

JsonValue ParseJsonFile(String file);
String SerializeJsonValue(JsonValue json);