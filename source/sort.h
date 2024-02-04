// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

#include "types.h"

template<typename Type>
struct SortAscending {
    bool operator()(const Type& a, const Type& b) {
        return a > b;
    }
};

template<typename Type>
struct SortDescending {
    bool operator()(const Type& a, const Type& b) {
        return a < b;
    }
};

template<typename Type>
void Swap(Type& one, Type& two) {
    Type temp = one;
    one = two;
    two = temp;
}

template<typename Type, typename Comparison = SortAscending<Type>>
void QSortRecursive(Type* pData, i64 low, i64 high, Comparison cmp) {
    if (low < high) {
        i64 pivot = low;  // TODO: Different pivot picking heuristic?
        i64 i = low;
        i64 j = high;

        while (i < j) {
            while (!cmp(pData[i], pData[pivot]) && i < high)
                i++;
            while (cmp(pData[j], pData[pivot]))
                j--;

            if (i < j)
                Swap(pData[i], pData[j]);
        }
        Swap(pData[pivot], pData[j]);
        QSortRecursive(pData, low, j - 1, cmp);
        QSortRecursive(pData, j + 1, high, cmp);
    } else {
        return;
    }
}

template<typename Type, typename Comparison = SortAscending<Type>>
void Sort(Type* pData, usize count, Comparison cmp = Comparison()) {
    QSortRecursive(pData, 0, count - 1, cmp);
}

template<typename Type, typename Comparison = SortAscending<Type>>
bool IsSorted(Type* pData, usize count, Comparison cmp = Comparison()) {
    if (count > 0) {
        for (int i = 0; i < count - 1; i++) {
            if (cmp(pData[i], pData[i + 1])) {
                return false;
            }
        }
    }
    return true;
}
