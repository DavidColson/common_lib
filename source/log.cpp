// Copyright 2020-2022 David Colson. All rights reserved.

#include "log.h"

#include "defer.h"
#include "platform_debug.h"
#include "string_builder.h"

#include <Windows.h>

namespace Log {
LogLevel globalLogLevel { Log::EDebug };
LogConfig config;
FILE* pLogFile { nullptr };

// ***********************************************************************

void PushLogMessage(LogLevel level, String message) {
    defer(FreeString(message));

    if (level > globalLogLevel)
        return;

    if (config.fileOutput) {
        if (pLogFile == nullptr)
            fopen_s(&pLogFile, "application.log", "w");
        fprintf(pLogFile, message.pData);
        fflush(pLogFile);
    }

    if (config.winOutput)
        OutputDebugStringA(message.pData);

    if (config.consoleOutput)
        printf("%s", message.pData);

    if (level <= Log::ECrit) {
        void* trace[100];
        size_t frames = PlatformDebug::CollectStackTrace(trace, 100, 2);

        String stackTrace = PlatformDebug::PrintStackTraceToString(trace, frames);
        if (config.fileOutput) {
            fprintf(pLogFile, stackTrace.pData);
            fflush(pLogFile);
        }
        if (config.winOutput)
            OutputDebugStringA(stackTrace.pData);
        if (config.consoleOutput)
            printf("%s", stackTrace.pData);
    }

    if (config.customHandler1)
        config.customHandler1(level, message);

    if (config.customHandler2)
        config.customHandler2(level, message);

    if (config.critCrashes && level <= Log::ECrit)
        __debugbreak();
}
}

// ***********************************************************************

void Log::SetConfig(LogConfig _config) {
    config = _config;
}

// ***********************************************************************

void Log::SetLogLevel(LogLevel level) {
    globalLogLevel = level;
}

// ***********************************************************************

void Log::Crit(const char* text, ...) {
    StringBuilder builder;
    builder.Append("[CRITICAL] ");
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
    builder.Append("[WARNING] ");
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
    builder.Append("[INFO] ");
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
    builder.Append("[DEBUG] ");
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
        builder.Append("[ASSERT FAIL] ");
        builder.Append(message);
        builder.Append("\n");

        String output = builder.CreateString();
        PushLogMessage(LogLevel::EAssert, output);
    }
}
