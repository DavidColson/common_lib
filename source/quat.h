// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

template<typename T>
struct Matrix;
template<typename T>
struct Vec3;

template<typename T>
struct Quat {
    T x;
    T y;
    T z;
    T w;

    Quat()
        : x(T()), y(T()), z(T()), w(T(1.0)) {}
    Quat(T x, T y, T z, T w)
        : x(x), y(y), z(z), w(w) {}

    Quat(Vec3<T> axis, T angle);

    inline static Quat Identity();

    inline static Quat MakeFromEuler(Vec3<T> v);

    inline static Quat MakeFromEuler(T x, T y, T z);

    inline Matrix<T> ToMatrix() const;

    inline Quat GetInverse() const;

    // Normalize
    inline void Normalize();

    inline Quat GetNormalized();

    // Quat * Quat
    inline Quat operator*(const Quat& rhs) const;

    inline Vec3<T> operator*(const Vec3<T> v) const;

    inline Vec3<T> GetForwardVector();

    inline Vec3<T> GetRightVector();

    inline Vec3<T> GetUpVector();

    inline Vec3<T> GetAxis();

    inline T GetAngle();

    inline Vec3<T> GetEulerAngles();

    // TO STRING
};

typedef Quat<float> Quatf;
typedef Quat<double> Quatd;
