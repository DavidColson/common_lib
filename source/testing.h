// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

#include <stdio.h>

// Testing helpers
// ------------------
// .... todo documentation, examples etc

void StartTest(const byte* testName) {
    printf("Starting test: %s\n", testName);
}

void EndTest(int errorCount) {
    if (errorCount == 0) {
        printf("PASS\n\n");
    } else {
        printf("Failed with %i errors\n", errorCount);
    }
    return;
}

bool _verify(bool expression, int& errorCount, const byte* file, int line, const byte* msg) {
    if (!expression) {
        errorCount++;
        printf("FAIL: %s(%i): %s\n", file, line, msg);
        return false;
    }
    return true;
}

#define VERIFY(expression) _verify((expression), errorCount, __FILE__, __LINE__, (#expression))
#define VERIFY_MSG(expression, msg) _verify((expression), errorCount, __FILE__, __LINE__, msg)
