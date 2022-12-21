// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

template<typename T>
struct Quat;
template<typename T>
struct Vec3;
template<typename T>
struct Vec4;

// @Improvement: properly comment/document the rest of this

/**
 * A 4x4 matrix
 */
template<typename T>
struct Matrix {
    T m[4][4];  // column, row (column major)

    Matrix();

    Matrix(const Matrix& copy);

    inline static Matrix Identity();

    /**
     * Matrix vs Matrix multiplication
     *
     * @param  rhs The matrix to multiple on the right of this
     * @return The result of the multiplication
     **/
    inline Matrix operator*(const Matrix& rhs) const;

    /**
     * Matrix vs Matrix multiplication, but this time assignment operator
     *
     * @param  rhs The matrix to multiple on the right of this
     * @return The result of the multiplication
     **/
    inline Matrix operator*=(const Matrix& rhs);

    /**
     * Matrix vs Vector multiplication
     *
     * @param  rhs The matrix to multiple on the right of this
     * @return The result of the multiplication
     **/
    inline Vec4<T> operator*(const Vec4<T>& rhs) const;

    /**
     * Matrix vs Vector3 multiplication. Note that this will implicitly convert rhs to a vec4 with w component equal to 1
     *
     * @param  rhs The matrix to multiple on the right of this
     * @return The result of the multiplication
     **/
    inline Vec3<T> operator*(const Vec3<T>& rhs) const;


    inline Matrix GetTranspose() const;

    inline T GetDeterminant() const;

    inline Matrix GetInverse() const;

    // Equivalent to Matrix.GetInverse() * Vec, but faster
    inline Vec4<T> InverseTransformVec(const Vec4<T>& rhs) const;

    inline Vec3<T> GetRightVector() const;

    inline Vec3<T> GetUpVector() const;

    inline Vec3<T> GetForwardVector() const;

    inline bool IsRotationOrthonormal() const;

    inline Vec3<T> ExtractScaling();

    inline Quat<T> ToQuat() const;

    inline void ToTQS(Vec3<T>& outTranslation, Quat<T> outOrientation, Vec3<T> outScale);

    inline void ToTRS(Vec3<T>& outTranslation, Vec3<T>& outEulerAngles, Vec3<T>& outScale);

    inline Vec3<T> GetScaling() const;

    inline void SetScaling(Vec3<T> scale);

    inline Vec3<T> GetEulerRotation() const;

    inline void SetEulerRotation(Vec3<T> rotation);

    inline void SetQuatRotation(Quat<T> rot);

    inline Vec3<T> GetTranslation() const;

    inline void SetTranslation(Vec3<T> translation);

    inline static Matrix MakeTRS(Vec3<T> translation, Vec3<T> eulerAngles, Vec3<T> scale);

    inline static Matrix MakeTQS(Vec3<T> translation, Quat<T> rot, Vec3<T> scale);

    inline static Matrix MakeTranslation(Vec3<T> translate);

    inline static Matrix MakeRotation(Vec3<T> rotation);

    inline static Matrix MakeScale(Vec3<T> scale);

    inline static Matrix Perspective(T screenWidth, T screenHeight, T nearPlane, T farPlane, T fov);

    inline static Matrix Orthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);

    inline static Matrix MakeLookAt(Vec3<T> Forward, Vec3<T> Up);
};

typedef Matrix<float> Matrixf;
typedef Matrix<double> Matrixd;
