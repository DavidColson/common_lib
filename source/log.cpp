// Copyright 2020-2022 David Colson. All rights reserved.

#include "log.h"

#include "defer.h"
#include "platform_debug.h"
#include "string_builder.h"
#include "light_string.h"

#include <stdio.h>
#include <stdarg.h>

// Windows defines
// ***********************************************************************

extern "C" {
#define WIN(r) __declspec(dllimport) r __stdcall
WIN(void) OutputDebugStringA(const char* lpOutputString);
}

// ***********************************************************************

namespace Log {
LogLevel g_logLevel { Log::EDebug };
LogConfig g_config;
FILE* g_pLogFile { nullptr };

// ***********************************************************************

void PushLogMessage(LogLevel level, String message) {
    defer(FreeString(message));

    if (level > g_logLevel)
        return;

    if (g_config.fileOutput) {
        if (g_pLogFile == nullptr)
            fopen_s(&g_pLogFile, "application.log", "w");
        fprintf(g_pLogFile, message.pData);
        fflush(g_pLogFile);
    }

    if (g_config.winOutput)
        OutputDebugStringA(message.pData);

    if (g_config.consoleOutput)
        printf("%s", message.pData);

    if (level <= Log::ECrit) {
        void* trace[100];
        usize frames = PlatformDebug::CollectStackTrace(trace, 100, 2);

        String stackTrace = PlatformDebug::PrintStackTraceToString(trace, frames);
        if (g_config.fileOutput) {
            fprintf(g_pLogFile, stackTrace.pData);
            fflush(g_pLogFile);
        }
        if (g_config.winOutput)
            OutputDebugStringA(stackTrace.pData);
        if (g_config.consoleOutput)
            printf("%s", stackTrace.pData);
    }

    if (g_config.customHandler1)
        g_config.customHandler1(level, message);

    if (g_config.customHandler2)
        g_config.customHandler2(level, message);

    if (g_config.critCrashes && level <= Log::ECrit)
        __debugbreak();
}
}

// ***********************************************************************

void Log::SetConfig(LogConfig _config) {
    g_config = _config;
}

// ***********************************************************************

void Log::SetLogLevel(LogLevel level) {
    g_logLevel = level;
}

// ***********************************************************************

void Log::Crit(const char* text, ...) {
    StringBuilder builder;
    if (!g_config.silencePrefixes) builder.Append("[CRITICAL] ");
    va_list arguments;
    va_start(arguments, text);
    builder.AppendFormatInternal(text, arguments);
    va_end(arguments);
    builder.Append("\n");

    String message = builder.CreateString();
    PushLogMessage(LogLevel::ECrit, message);
}

// ***********************************************************************

void Log::Warn(const char* text, ...) {
    StringBuilder builder;
	if (!g_config.silencePrefixes) builder.Append("[WARNING] ");
    va_list arguments;
    va_start(arguments, text);
    builder.AppendFormatInternal(text, arguments);
    va_end(arguments);
    builder.Append("\n");

    String message = builder.CreateString();
    PushLogMessage(LogLevel::EWarn, message);
}

// ***********************************************************************

void Log::Info(const char* text, ...) {
    StringBuilder builder;
	if (!g_config.silencePrefixes) builder.Append("[INFO] ");
    va_list arguments;
    va_start(arguments, text);
    builder.AppendFormatInternal(text, arguments);
    va_end(arguments);
    builder.Append("\n");

    String message = builder.CreateString();
    PushLogMessage(LogLevel::EInfo, message);
}

// ***********************************************************************

void Log::Debug(const char* text, ...) {
    StringBuilder builder;
	if (!g_config.silencePrefixes) builder.Append("[DEBUG] ");
    va_list arguments;
    va_start(arguments, text);
    builder.AppendFormatInternal(text, arguments);
    va_end(arguments);
    builder.Append("\n");

    String message = builder.CreateString();
    PushLogMessage(LogLevel::EDebug, message);
}

// ***********************************************************************

void Log::_Assertion(bool expression, const char* message) {
    if (!expression) {
        StringBuilder builder;
		if (!g_config.silencePrefixes) builder.Append("[ASSERT FAIL] ");
        builder.Append(message);
        builder.Append("\n");

        String output = builder.CreateString();
        PushLogMessage(LogLevel::EAssert, output);
    }
}
