// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

#include "matrix.h"
#include "vec3.h"

template<typename T>
struct AABB {
    Vec3<T> min;
    Vec3<T> max;

    // Probably want merge for mesh combining primitives?
};

template<typename T>
AABB<T> TransformAABB(AABB<T> a, Matrix<T> m) {
    AABB<T> res;
    Vec3<T> sca = m.ExtractScaling();
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            f32 e = m.m[i][j] * a.min[j];
            f32 f = m.m[i][j] * a.max[j];
            if (e < f) {
                res.min[i] += e;
                res.max[i] += f;
            } else {
                res.min[i] += f;
                res.max[i] += e;
            }
        }
        res.min[i] *= sca[i];
        res.max[i] *= sca[i];
        res.min[i] += m.m[i][3];
        res.max[i] += m.m[i][3];
    }
    return res;
}

typedef AABB<f32> AABBf;
typedef AABB<f64> AABBd;
