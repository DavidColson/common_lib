// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

#define PI 3.141592653589793238f
#define PI2 6.283185307179586476f

#define ToRadian(x) ((x)*3.14159f / 180.0f)
#define ToDegree(x) ((x)*180.0f / 3.14159f)

inline f32 LinearMap(f32 x, f32 fromMin, f32 fromMax, f32 toMin, f32 toMax) {
    return toMin + ((x - fromMin) / (fromMax - fromMin)) * (toMax - toMin);
}

inline i32 mod_floor(i32 a, i32 n) {
    return ((a % n) + n) % n;
}

inline i32 mod_floor(i32 a, u64 n) {
    return mod_floor(a, (i32)n);
}

template<typename T>
inline T min(T a, T b) {
	return a < b ? a : b;
}

template<typename T>
inline T max(T a, T b) {
	return a > b ? a : b;
}

template<typename T>
inline T clamp(T val, T min, T max) {
    if (val < min)
        return min;
    if (val > max)
        return max;
    return val;
}

f64 generateGaussian(f64 mean, f64 stdDev);
