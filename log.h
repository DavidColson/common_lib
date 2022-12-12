#pragma once

#include "string.h"

namespace Log
{
	enum LogLevel
	{
		EAssert,
		ECrit,
		EWarn,
		EInfo,
		EDebug
	};

	struct LogConfig {
		bool winOutput { true };
		bool consoleOutput{ true };
		bool fileOutput{ true };
		bool critCrashes{ true };
		
		void (*customHandler1)(LogLevel, String);
		void (*customHandler2)(LogLevel, String);
	};

	void SetConfig(LogConfig config);
	void SetLogLevel(LogLevel level);

	void Crit(const char* text, ...);
	void Warn(const char* text, ...);
	void Info(const char* text, ...);
	void Debug(const char* text, ...);

	void _Assertion(bool expression, const char* message);
}

#ifdef _DEBUG
#define Assert(expression) Log::_Assertion((expression), (#expression))
#define AssertMsg(expression, msg) Log::_Assertion((expression), msg)
#else
#define Assert(expression)
#define AssertMsg(expression, msg)
#endif