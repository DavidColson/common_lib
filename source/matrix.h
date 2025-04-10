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

    inline static Matrix MakeRotation(T angle, Vec3<T> rotation);

    inline static Matrix MakeScale(Vec3<T> scale);

    inline static Matrix Perspective(T screenWidth, T screenHeight, T nearPlane, T farPlane, T fov);

    inline static Matrix Orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 nearPlane, f32 farPlane);

    inline static Matrix MakeLookAt(Vec3<T> Forward, Vec3<T> Up);
};

typedef Matrix<f32> Matrixf;
typedef Matrix<f64> Matrixd;


// IMPLEMENTATION

// ***********************************************************************

template<typename T>
Matrix<T>::Matrix() {
    m[0][0] = T(1.0);
    m[1][0] = T(0.0);
    m[2][0] = T(0.0);
    m[3][0] = T(0.0);
    m[0][1] = T(0.0);
    m[1][1] = T(1.0);
    m[2][1] = T(0.0);
    m[3][1] = T(0.0);
    m[0][2] = T(0.0);
    m[1][2] = T(0.0);
    m[2][2] = T(1.0);
    m[3][2] = T(0.0);
    m[0][3] = T(0.0);
    m[1][3] = T(0.0);
    m[2][3] = T(0.0);
    m[3][3] = T(1.0);
}

// ***********************************************************************

template<typename T>
Matrix<T>::Matrix(const Matrix& copy) {
    m[0][0] = copy.m[0][0];
    m[1][0] = copy.m[1][0];
    m[2][0] = copy.m[2][0];
    m[3][0] = copy.m[3][0];
    m[0][1] = copy.m[0][1];
    m[1][1] = copy.m[1][1];
    m[2][1] = copy.m[2][1];
    m[3][1] = copy.m[3][1];
    m[0][2] = copy.m[0][2];
    m[1][2] = copy.m[1][2];
    m[2][2] = copy.m[2][2];
    m[3][2] = copy.m[3][2];
    m[0][3] = copy.m[0][3];
    m[1][3] = copy.m[1][3];
    m[2][3] = copy.m[2][3];
    m[3][3] = copy.m[3][3];
}

// ***********************************************************************

template<typename T>
inline Matrix<T> Matrix<T>::Identity() {
    Matrix mat;
    mat.m[0][0] = T(1.0);
    mat.m[1][0] = T(0.0);
    mat.m[2][0] = T(0.0);
    mat.m[3][0] = T(0.0);
    mat.m[0][1] = T(0.0);
    mat.m[1][1] = T(1.0);
    mat.m[2][1] = T(0.0);
    mat.m[3][1] = T(0.0);
    mat.m[0][2] = T(0.0);
    mat.m[1][2] = T(0.0);
    mat.m[2][2] = T(1.0);
    mat.m[3][2] = T(0.0);
    mat.m[0][3] = T(0.0);
    mat.m[1][3] = T(0.0);
    mat.m[2][3] = T(0.0);
    mat.m[3][3] = T(1.0);
    return mat;
}

// ***********************************************************************

template<typename T>
inline Matrix<T> Matrix<T>::operator*(const Matrix<T>& rhs) const {
    Matrix<T> mat;

    // Row 1
    mat.m[0][0] = m[0][0] * rhs.m[0][0] + m[1][0] * rhs.m[0][1] + m[2][0] * rhs.m[0][2] + m[3][0] * rhs.m[0][3];
    mat.m[1][0] = m[0][0] * rhs.m[1][0] + m[1][0] * rhs.m[1][1] + m[2][0] * rhs.m[1][2] + m[3][0] * rhs.m[1][3];
    mat.m[2][0] = m[0][0] * rhs.m[2][0] + m[1][0] * rhs.m[2][1] + m[2][0] * rhs.m[2][2] + m[3][0] * rhs.m[2][3];
    mat.m[3][0] = m[0][0] * rhs.m[3][0] + m[1][0] * rhs.m[3][1] + m[2][0] * rhs.m[3][2] + m[3][0] * rhs.m[3][3];

    // Row 2
    mat.m[0][1] = m[0][1] * rhs.m[0][0] + m[1][1] * rhs.m[0][1] + m[2][1] * rhs.m[0][2] + m[3][1] * rhs.m[0][3];
    mat.m[1][1] = m[0][1] * rhs.m[1][0] + m[1][1] * rhs.m[1][1] + m[2][1] * rhs.m[1][2] + m[3][1] * rhs.m[1][3];
    mat.m[2][1] = m[0][1] * rhs.m[2][0] + m[1][1] * rhs.m[2][1] + m[2][1] * rhs.m[2][2] + m[3][1] * rhs.m[2][3];
    mat.m[3][1] = m[0][1] * rhs.m[3][0] + m[1][1] * rhs.m[3][1] + m[2][1] * rhs.m[3][2] + m[3][1] * rhs.m[3][3];

    // Row 3
    mat.m[0][2] = m[0][2] * rhs.m[0][0] + m[1][2] * rhs.m[0][1] + m[2][2] * rhs.m[0][2] + m[3][2] * rhs.m[0][3];
    mat.m[1][2] = m[0][2] * rhs.m[1][0] + m[1][2] * rhs.m[1][1] + m[2][2] * rhs.m[1][2] + m[3][2] * rhs.m[1][3];
    mat.m[2][2] = m[0][2] * rhs.m[2][0] + m[1][2] * rhs.m[2][1] + m[2][2] * rhs.m[2][2] + m[3][2] * rhs.m[2][3];
    mat.m[3][2] = m[0][2] * rhs.m[3][0] + m[1][2] * rhs.m[3][1] + m[2][2] * rhs.m[3][2] + m[3][2] * rhs.m[3][3];

    // Row 4
    mat.m[0][3] = m[0][3] * rhs.m[0][0] + m[1][3] * rhs.m[0][1] + m[2][3] * rhs.m[0][2] + m[3][3] * rhs.m[0][3];
    mat.m[1][3] = m[0][3] * rhs.m[1][0] + m[1][3] * rhs.m[1][1] + m[2][3] * rhs.m[1][2] + m[3][3] * rhs.m[1][3];
    mat.m[2][3] = m[0][3] * rhs.m[2][0] + m[1][3] * rhs.m[2][1] + m[2][3] * rhs.m[2][2] + m[3][3] * rhs.m[2][3];
    mat.m[3][3] = m[0][3] * rhs.m[3][0] + m[1][3] * rhs.m[3][1] + m[2][3] * rhs.m[3][2] + m[3][3] * rhs.m[3][3];
    return mat;
}

// ***********************************************************************

template<typename T>
inline Matrix<T> Matrix<T>::operator*=(const Matrix<T>& rhs) {
    *this = operator*(rhs);
    return *this;
}

// ***********************************************************************

template<typename T>
inline Vec4<T> Matrix<T>::operator*(const Vec4<T>& rhs) const {
    Vec4<T> vec;

    vec.x = m[0][0] * rhs.x + m[1][0] * rhs.y + m[2][0] * rhs.z + m[3][0] * rhs.w;
    vec.y = m[0][1] * rhs.x + m[1][1] * rhs.y + m[2][1] * rhs.z + m[3][1] * rhs.w;
    vec.z = m[0][2] * rhs.x + m[1][2] * rhs.y + m[2][2] * rhs.z + m[3][2] * rhs.w;
    vec.w = m[0][3] * rhs.x + m[1][3] * rhs.y + m[2][3] * rhs.z + m[3][3] * rhs.w;

    return vec;
}

// ***********************************************************************

template<typename T>
inline Vec3<T> Matrix<T>::operator*(const Vec3<T>& rhs) const {
    Vec3<T> vec;

    vec.x = m[0][0] * rhs.x + m[1][0] * rhs.y + m[2][0] * rhs.z + m[3][0];
    vec.y = m[0][1] * rhs.x + m[1][1] * rhs.y + m[2][1] * rhs.z + m[3][1];
    vec.z = m[0][2] * rhs.x + m[1][2] * rhs.y + m[2][2] * rhs.z + m[3][2];

    return vec;
}

// ***********************************************************************

template<typename T>
inline Matrix<T> Matrix<T>::GetTranspose() const {
    Matrix<T> mat;
    mat.m[0][0] = m[0][0];
    mat.m[1][0] = m[0][1];
    mat.m[2][0] = m[0][2];
    mat.m[3][0] = m[0][3];

    mat.m[0][1] = m[1][0];
    mat.m[1][1] = m[1][1];
    mat.m[2][1] = m[1][2];
    mat.m[3][1] = m[1][3];

    mat.m[0][2] = m[2][0];
    mat.m[1][2] = m[2][1];
    mat.m[2][2] = m[2][2];
    mat.m[3][2] = m[2][3];

    mat.m[0][3] = m[3][0];
    mat.m[1][3] = m[3][1];
    mat.m[2][3] = m[3][2];
    mat.m[3][3] = m[3][3];
    return mat;
}

// ***********************************************************************

template<typename T>
inline T Matrix<T>::GetDeterminant() const {
    return m[0][0] * (m[1][1] * m[2][2] * m[3][3] + m[2][1] * m[3][2] * m[1][3] + m[3][1] * m[1][2] * m[2][3] - m[3][1] * m[2][2] * m[1][3] - m[2][1] * m[1][2] * m[3][3] - m[1][1] * m[3][2] * m[2][3])
           - m[1][0] * (m[0][1] * m[2][2] * m[3][3] + m[2][1] * m[3][2] * m[0][3] + m[3][1] * m[0][2] * m[2][3] - m[3][1] * m[2][2] * m[0][3] - m[2][1] * m[0][2] * m[3][3] - m[0][1] * m[3][2] * m[2][3])
           + m[2][0] * (m[0][1] * m[1][2] * m[3][3] + m[1][1] * m[3][2] * m[0][3] + m[3][1] * m[0][2] * m[1][3] - m[3][1] * m[1][2] * m[0][3] - m[1][1] * m[0][2] * m[3][3] - m[0][1] * m[3][2] * m[1][3])
           - m[3][0] * (m[0][1] * m[1][2] * m[2][1] + m[1][1] * m[2][2] * m[0][3] + m[2][1] * m[0][2] * m[1][3] - m[2][1] * m[1][2] * m[0][3] - m[1][1] * m[0][2] * m[2][3] - m[0][1] * m[2][2] * m[1][3]);
}

// ***********************************************************************

template<typename T>
inline Matrix<T> Matrix<T>::GetInverse() const {
    Matrix<T> res;
    f32 iDet = 1.0f / GetDeterminant();

    res.m[0][0] = iDet * (m[1][1] * m[2][2] * m[3][3] + m[2][1] * m[3][2] * m[1][3] + m[3][1] * m[1][2] * m[2][3] - m[3][1] * m[2][2] * m[1][3] - m[2][1] * m[1][2] * m[3][3] - m[1][1] * m[3][2] * m[2][3]);
    res.m[1][0] = iDet * (-m[1][0] * m[2][2] * m[3][3] - m[2][0] * m[3][2] * m[1][3] - m[3][0] * m[1][2] * m[2][3] + m[3][0] * m[2][2] * m[1][3] + m[2][0] * m[1][2] * m[3][3] + m[1][0] * m[3][2] * m[2][3]);
    res.m[2][0] = iDet * (m[1][0] * m[2][1] * m[3][3] + m[2][0] * m[3][1] * m[1][3] + m[3][0] * m[1][1] * m[2][3] - m[3][0] * m[2][1] * m[1][3] - m[2][0] * m[1][1] * m[3][3] - m[1][0] * m[3][1] * m[2][3]);
    res.m[3][0] = iDet * (-m[1][0] * m[2][1] * m[3][2] - m[2][0] * m[3][1] * m[1][2] - m[3][0] * m[1][1] * m[2][2] + m[3][0] * m[2][1] * m[1][2] + m[2][0] * m[1][1] * m[3][2] + m[1][0] * m[3][1] * m[2][2]);

    res.m[0][1] = iDet * (-m[0][1] * m[2][2] * m[3][3] - m[2][1] * m[3][2] * m[0][3] - m[3][1] * m[0][2] * m[2][3] + m[3][1] * m[2][2] * m[0][3] + m[2][1] * m[0][2] * m[3][3] + m[0][1] * m[3][2] * m[2][3]);
    res.m[1][1] = iDet * (m[0][0] * m[2][2] * m[3][3] + m[2][0] * m[3][2] * m[0][3] + m[3][0] * m[0][2] * m[2][3] - m[3][0] * m[2][2] * m[0][3] - m[2][0] * m[0][2] * m[3][3] - m[0][0] * m[3][2] * m[2][3]);
    res.m[2][1] = iDet * (-m[0][0] * m[2][1] * m[3][3] - m[2][0] * m[3][1] * m[0][3] - m[3][0] * m[0][1] * m[2][3] + m[3][0] * m[2][1] * m[0][3] + m[2][0] * m[0][1] * m[3][3] + m[0][0] * m[3][1] * m[2][3]);
    res.m[3][1] = iDet * (m[0][0] * m[2][1] * m[3][2] + m[2][0] * m[3][1] * m[0][2] + m[3][0] * m[0][1] * m[2][2] - m[3][0] * m[2][1] * m[0][2] - m[2][0] * m[0][1] * m[3][2] - m[0][0] * m[3][1] * m[2][2]);

    res.m[0][2] = iDet * (m[0][1] * m[1][2] * m[3][3] + m[1][1] * m[3][2] * m[0][3] + m[3][1] * m[0][2] * m[1][3] - m[3][1] * m[1][2] * m[0][3] - m[1][1] * m[0][2] * m[3][3] - m[0][1] * m[3][2] * m[1][3]);
    res.m[1][2] = iDet * (-m[0][0] * m[1][2] * m[3][3] - m[1][0] * m[3][2] * m[0][3] - m[3][0] * m[0][2] * m[1][3] + m[3][0] * m[1][2] * m[0][3] + m[1][0] * m[0][2] * m[3][3] + m[0][0] * m[3][2] * m[1][3]);
    res.m[2][2] = iDet * (m[0][0] * m[1][1] * m[3][3] + m[1][0] * m[3][1] * m[0][3] + m[3][0] * m[0][1] * m[1][3] - m[3][0] * m[1][1] * m[0][3] - m[1][0] * m[0][1] * m[3][3] - m[0][0] * m[3][1] * m[1][3]);
    res.m[3][2] = iDet * (-m[0][0] * m[1][1] * m[3][2] - m[1][0] * m[3][1] * m[0][2] - m[3][0] * m[0][1] * m[1][2] + m[3][0] * m[1][1] * m[0][2] + m[1][0] * m[0][1] * m[3][2] + m[0][0] * m[3][1] * m[1][2]);

    res.m[0][3] = iDet * (-m[0][1] * m[1][2] * m[2][3] - m[1][1] * m[2][2] * m[0][3] - m[2][1] * m[0][2] * m[1][3] + m[2][1] * m[1][2] * m[0][3] + m[1][1] * m[0][2] * m[2][3] + m[0][1] * m[2][2] * m[1][3]);
    res.m[1][3] = iDet * (m[0][0] * m[1][2] * m[2][3] + m[1][0] * m[2][2] * m[0][3] + m[2][0] * m[0][2] * m[1][3] - m[2][0] * m[1][2] * m[0][3] - m[1][0] * m[0][2] * m[2][3] - m[0][0] * m[2][2] * m[1][3]);
    res.m[2][3] = iDet * (-m[0][0] * m[1][1] * m[2][3] - m[1][0] * m[2][1] * m[0][3] - m[2][0] * m[0][1] * m[1][3] + m[2][0] * m[1][1] * m[0][3] + m[1][0] * m[0][1] * m[2][3] + m[0][0] * m[2][1] * m[1][3]);
    res.m[3][3] = iDet * (m[0][0] * m[1][1] * m[2][2] + m[1][0] * m[2][1] * m[0][2] + m[2][0] * m[0][1] * m[1][2] - m[2][0] * m[1][1] * m[0][2] - m[1][0] * m[0][1] * m[2][2] - m[0][0] * m[2][1] * m[1][2]);
    return res;
}

// ***********************************************************************

template<typename T>
inline Vec4<T> Matrix<T>::InverseTransformVec(const Vec4<T>& rhs) const {
    Matrix<T> LU;

    LU.m[0][0] = m[0][0];
    LU.m[0][1] = m[0][1];
    LU.m[0][2] = m[0][2];
    LU.m[0][3] = m[0][3];

    LU.m[1][0] = m[1][0] / LU.m[0][0];
    LU.m[2][0] = m[2][0] / LU.m[0][0];
    LU.m[3][0] = m[3][0] / LU.m[0][0];

    LU.m[1][1] = m[1][1] - LU.m[0][1] * LU.m[1][0];
    LU.m[1][2] = m[1][2] - LU.m[0][2] * LU.m[1][0];
    LU.m[1][3] = m[1][3] - LU.m[0][3] * LU.m[1][0];

    LU.m[2][1] = (m[2][1] - LU.m[0][1] * LU.m[2][0]) / LU.m[1][1];
    LU.m[3][1] = (m[3][1] - LU.m[0][1] * LU.m[3][0]) / LU.m[1][1];

    LU.m[2][2] = m[2][2] - (LU.m[0][2] * LU.m[2][0] + LU.m[1][2] * LU.m[2][1]);
    LU.m[2][3] = m[2][3] - (LU.m[0][3] * LU.m[2][0] + LU.m[1][3] * LU.m[2][1]);

    LU.m[3][2] = (m[3][2] - (LU.m[0][2] * LU.m[3][0] + LU.m[1][2] * LU.m[3][1])) / LU.m[2][2];

    LU.m[3][3] = m[3][3] - (LU.m[0][3] * LU.m[3][0] + LU.m[1][3] * LU.m[3][1] + LU.m[2][3] * LU.m[3][2]);

    f32 d_0 = rhs.x / LU.m[0][0];
    f32 d_1 = (rhs.y - LU.m[0][1] * d_0) / LU.m[1][1];
    f32 d_2 = (rhs.z - LU.m[0][2] * d_0 - LU.m[1][2] * d_1) / LU.m[2][2];
    f32 d_3 = (rhs.w - LU.m[0][3] * d_0 - LU.m[1][3] * d_1 - LU.m[2][3] * d_2) / LU.m[3][3];

    Vec4<T> vec;

    vec.w = d_3;
    vec.z = d_2 - (LU.m[2][3] * vec.w);
    vec.y = d_1 - (LU.m[2][1] * vec.z + LU.m[3][1] * vec.w);
    vec.x = d_0 - (LU.m[1][0] * vec.y + LU.m[2][0] * vec.z + LU.m[3][0] * vec.w);

    return vec;
}

// ***********************************************************************

template<typename T>
inline Vec3<T> Matrix<T>::GetRightVector() const {
    Vec3<T> vec;
    vec.x = m[0][0];
    vec.y = m[0][1];
    vec.z = m[0][2];
    return vec;
}

// ***********************************************************************

template<typename T>
inline Vec3<T> Matrix<T>::GetUpVector() const {
    Vec3<T> vec;
    vec.x = m[1][0];
    vec.y = m[1][1];
    vec.z = m[1][2];
    return vec;
}

// ***********************************************************************

template<typename T>
inline Vec3<T> Matrix<T>::GetForwardVector() const {
    Vec3<T> vec;
    vec.x = m[2][0];
    vec.y = m[2][1];
    vec.z = m[2][2];
    return vec;
}

// ***********************************************************************

template<typename T>
inline bool Matrix<T>::IsRotationOrthonormal() const {
    bool a = Vec3<T>::IsEquivalent(GetRightVector(), Vec3<T>::Cross(GetUpVector(), GetForwardVector()));
    bool b = Vec3<T>::IsEquivalent(GetUpVector(), Vec3<T>::Cross(GetForwardVector(), GetRightVector()));
    bool c = Vec3<T>::IsEquivalent(GetForwardVector(), Vec3<T>::Cross(GetRightVector(), GetUpVector()));
    return a && b && c;
}

// ***********************************************************************

template<typename T>
inline Vec3<T> Matrix<T>::ExtractScaling() {
    // Completely removes scaling from this matrix and returns the scale vector
    Vec3<T> sca;
    sca.x = sqrtf(m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0]);
    sca.y = sqrtf(m[0][1] * m[0][1] + m[1][1] * m[1][1] + m[2][1] * m[2][1]);
    sca.z = sqrtf(m[0][2] * m[0][2] + m[1][2] * m[1][2] + m[2][2] * m[2][2]);

    T invScaX = T(1.0) / sca.x;
    m[0][0] *= invScaX;
    m[1][0] *= invScaX;
    m[2][0] *= invScaX;

    T invScaY = T(1.0) / sca.y;
    m[0][1] *= invScaY;
    m[1][1] *= invScaY;
    m[2][1] *= invScaY;

    T invScaZ = T(1.0) / sca.z;
    m[0][2] *= invScaZ;
    m[1][2] *= invScaZ;
    m[2][2] *= invScaZ;

    return sca;
}

// ***********************************************************************

template<typename T>
inline Quat<T> Matrix<T>::ToQuat() const {
    // http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/

    // ASSERT(IsRotationOrthonormal(), "Cannot convert a non-orthonormal matrix to a quat");

    Quat<T> q;
    T trace = m[0][0] + m[1][1] + m[2][2];
    if (trace > 0) {
        T s = T(0.5) / sqrtf(trace + T(1.0));  // 4 * q.w
        q.w = T(0.25) / s;
        q.x = (m[1][2] - m[2][1]) * s;
        q.y = (m[2][0] - m[0][2]) * s;
        q.z = (m[0][1] - m[1][0]) * s;
    } else if (m[0][0] > m[1][1] && m[0][0] > m[2][2]) {
        T s = T(2.0) * sqrtf(T(1.0) + m[0][0] - m[1][1] - m[2][2]);
        q.w = (m[1][2] - m[2][1]) / s;
        q.x = T(0.25) * s;
        q.y = (m[1][0] + m[0][1]) / s;
        q.z = (m[2][0] + m[0][2]) / s;
    } else if (m[1][1] > m[2][2]) {
        T s = T(2.0) * sqrtf(T(1.0) + m[1][1] - m[0][0] - m[2][2]);
        q.w = (m[2][0] - m[0][2]) / s;
        q.x = (m[1][0] + m[0][1]) / s;
        q.y = T(0.25) * s;
        q.z = (m[2][1] + m[1][2]) / s;
    } else {
        T s = T(2.0) * sqrtf(T(1.0) + m[2][2] - m[0][0] - m[1][1]);
        q.w = (m[0][1] - m[1][0]) / s;
        q.x = (m[2][0] + m[0][1]) / s;
        q.y = (m[2][1] + m[1][2]) / s;
        q.z = T(0.25) * s;
    }
    return q;
}

// ***********************************************************************

template<typename T>
inline void Matrix<T>::ToTQS(Vec3<T>& outTranslation, Quat<T> outOrientation, Vec3<T> outScale) {
    Matrix<T> m = *this;
    // Extract scaling first from each row
    outScale = m.ExtractScaling();

    // Special case for negative scaling
    if (m.GetDeterminant() < 0.f) {
        // choice of axis to flip makes no difference
        outScale.x *= -1.f;
        m.m[0][0] = -m.m[0][0];
        m.m[1][0] = -m.m[1][0];
        m.m[2][0] = -m.m[2][0];
    }
    outOrientation = m.ToQuat();

    // Translation is easy, we just take the 4th column
    outTranslation.x = m.m[3][0];
    outTranslation.y = m.m[3][1];
    outTranslation.z = m.m[3][2];
}

// ***********************************************************************

template<typename T>
inline void Matrix<T>::ToTRS(Vec3<T>& outTranslation, Vec3<T>& outEulerAngles, Vec3<T>& outScale) {
    Matrix<T> m = *this;
    // Extract scaling first from each row
    outScale = m.ExtractScaling();

    // Special case for negative scaling
    if (m.GetDeterminant() < 0.f) {
        // choice of axis to flip makes no difference
        outScale.x *= -1.f;
        m.m[0][0] = -m.m[0][0];
        m.m[1][0] = -m.m[1][0];
        m.m[2][0] = -m.m[2][0];
    }
    outEulerAngles = m.ToQuat().GetEulerAngles();

    // Translation is easy, we just take the 4th column
    outTranslation.x = m.m[3][0];
    outTranslation.y = m.m[3][1];
    outTranslation.z = m.m[3][2];
}

// ***********************************************************************

template<typename T>
inline Vec3<T> Matrix<T>::GetScaling() const {
    Matrix<T> m = *this;
    return m.ExtractScaling();
}

// ***********************************************************************

template<typename T>
inline void Matrix<T>::SetScaling(Vec3<T> scale) {
    ExtractScaling();

    m[0][0] *= scale.x;
    m[1][0] *= scale.y;
    m[2][0] *= scale.z;

    m[0][1] *= scale.x;
    m[1][1] *= scale.y;
    m[2][1] *= scale.z;

    m[0][2] *= scale.x;
    m[1][2] *= scale.y;
    m[2][2] *= scale.z;
}

// ***********************************************************************

template<typename T>
inline Vec3<T> Matrix<T>::GetEulerRotation() const {
    Matrix<T> m = *this;
    // Extract scaling first from each row
    m.ExtractScaling();

    // Special case for negative scaling
    if (m.GetDeterminant() < 0.f)
        m.m[0][0] = -m.m[0][0];
    m.m[1][0] = -m.m[1][0];
    m.m[2][0] = -m.m[2][0];

    return m.ToQuat().GetEulerAngles();
}

// ***********************************************************************

template<typename T>
inline void Matrix<T>::SetEulerRotation(Vec3<T> rotation) {
    Vec3<T> scale = ExtractScaling();
    // This is a body 3-2-1 (z, then y, then x) rotation
    const T cx = cosf(rotation.x);
    const T sx = sinf(rotation.x);
    const T cy = cosf(rotation.y);
    const T sy = sinf(rotation.y);
    const T cz = cosf(rotation.z);
    const T sz = sinf(rotation.z);

    // TODO: Does this work with negative scaling?
    m[0][0] = cy * cz * scale.x;
    m[1][0] = (-cx * sz + sx * sy * cz) * scale.y;
    m[2][0] = (sx * sz + cx * sy * cz) * scale.z;

    m[0][1] = cy * sz * scale.x;
    m[1][1] = (cx * cz + sx * sy * sz) * scale.y;
    m[2][1] = (-sx * cz + cx * sy * sz) * scale.z;

    m[0][2] = -sy * scale.x;
    m[1][2] = sx * cy * scale.y;
    m[2][2] = cx * cy * scale.z;
}

// ***********************************************************************

template<typename T>
inline void Matrix<T>::SetQuatRotation(Quat<T> rot) {
    Vec3<T> scale = ExtractScaling();

    // TODO: Does this work with negative scaling?
    m[0][0] = (1.0f - 2.0f * rot.y * rot.y - 2.0f * rot.z * rot.z) * scale.x;
    m[0][1] = (2.0f * rot.x * rot.y + 2.0f * rot.z * rot.w) * scale.x;
    m[0][2] = (2.0f * rot.x * rot.z - 2.0f * rot.y * rot.w) * scale.x;

    m[1][0] = (2.0f * rot.x * rot.y - 2.0f * rot.z * rot.w) * scale.y;
    m[1][1] = (1.0f - 2.0f * rot.x * rot.x - 2.0f * rot.z * rot.z) * scale.y;
    m[1][2] = (2.0f * rot.y * rot.z + 2.0f * rot.x * rot.w) * scale.y;

    m[2][0] = (2.0f * rot.x * rot.z + 2.0f * rot.y * rot.w) * scale.z;
    m[2][1] = (2.0f * rot.y * rot.z - 2.0f * rot.x * rot.w) * scale.z;
    m[2][2] = (1.0f - 2.0f * rot.x * rot.x - 2.0f * rot.y * rot.y) * scale.z;
}

// ***********************************************************************

template<typename T>
inline Vec3<T> Matrix<T>::GetTranslation() const {
    Vec3<T> outTrans;

    outTrans.x = m[3][0];
    outTrans.y = m[3][1];
    outTrans.z = m[3][2];

    return outTrans;
}

template<typename T>
inline void Matrix<T>::SetTranslation(Vec3<T> translation) {
    m[3][0] = translation.x;
    m[3][1] = translation.y;
    m[3][2] = translation.z;
}

// ***********************************************************************

template<typename T>
inline static Matrix<T> Matrix<T>::MakeTRS(Vec3<T> translation, Vec3<T> eulerAngles, Vec3<T> scale) {
    // This is a body 3-2-1 (z, then y, then x) rotation
    const T cx = cos(eulerAngles.x);
    const T sx = sin(eulerAngles.x);
    const T cy = cos(eulerAngles.y);
    const T sy = sin(eulerAngles.y);
    const T cz = cos(eulerAngles.z);
    const T sz = sin(eulerAngles.z);

    Matrix<T> res;
    res.m[0][0] = cy * cz * scale.x;
    res.m[1][0] = (-cx * sz + sx * sy * cz) * scale.y;
    res.m[2][0] = (sx * sz + cx * sy * cz) * scale.z;
    res.m[3][0] = translation.x;

    res.m[0][1] = cy * sz * scale.x;
    res.m[1][1] = (cx * cz + sx * sy * sz) * scale.y;
    res.m[2][1] = (-sx * cz + cx * sy * sz) * scale.z;
    res.m[3][1] = translation.y;

    res.m[0][2] = -sy * scale.x;
    res.m[1][2] = sx * cy * scale.y;
    res.m[2][2] = cx * cy * scale.z;
    res.m[3][2] = translation.z;

    res.m[0][3] = T(0.0);
    res.m[1][3] = T(0.0);
    res.m[2][3] = T(0.0);
    res.m[3][3] = T(1.0);
    return res;
}

// ***********************************************************************

template<typename T>
inline static Matrix<T> Matrix<T>::MakeTQS(Vec3<T> translation, Quat<T> rot, Vec3<T> scale) {
    Matrix<T> mat;
    mat.m[0][0] = (1.0f - 2.0f * rot.y * rot.y - 2.0f * rot.z * rot.z) * scale.x;
    mat.m[0][1] = (2.0f * rot.x * rot.y + 2.0f * rot.z * rot.w) * scale.x;
    mat.m[0][2] = (2.0f * rot.x * rot.z - 2.0f * rot.y * rot.w) * scale.x;
    mat.m[0][3] = 0.0f;

    mat.m[1][0] = (2.0f * rot.x * rot.y - 2.0f * rot.z * rot.w) * scale.y;
    mat.m[1][1] = (1.0f - 2.0f * rot.x * rot.x - 2.0f * rot.z * rot.z) * scale.y;
    mat.m[1][2] = (2.0f * rot.y * rot.z + 2.0f * rot.x * rot.w) * scale.y;
    mat.m[1][3] = 0.0f;

    mat.m[2][0] = (2.0f * rot.x * rot.z + 2.0f * rot.y * rot.w) * scale.z;
    mat.m[2][1] = (2.0f * rot.y * rot.z - 2.0f * rot.x * rot.w) * scale.z;
    mat.m[2][2] = (1.0f - 2.0f * rot.x * rot.x - 2.0f * rot.y * rot.y) * scale.z;
    mat.m[2][3] = 0.0f;

    mat.m[3][0] = translation.x;
    mat.m[3][1] = translation.y;
    mat.m[3][2] = translation.z;
    mat.m[3][3] = 1.0f;
    return mat;
}

// ***********************************************************************

template<typename T>
inline Matrix<T> Matrix<T>::MakeTranslation(Vec3<T> translate) {
    Matrix<T> mat;
    mat.m[0][0] = T(1.0);
    mat.m[1][0] = T(0.0);
    mat.m[2][0] = T(0.0);
    mat.m[3][0] = translate.x;

    mat.m[0][1] = T(0.0);
    mat.m[1][1] = T(1.0);
    mat.m[2][1] = T(0.0);
    mat.m[3][1] = translate.y;

    mat.m[0][2] = T(0.0);
    mat.m[1][2] = T(0.0);
    mat.m[2][2] = T(1.0);
    mat.m[3][2] = translate.z;

    mat.m[0][3] = T(0.0);
    mat.m[1][3] = T(0.0);
    mat.m[2][3] = T(0.0);
    mat.m[3][3] = T(1.0);
    return mat;
}

// ***********************************************************************

template<typename T>
inline Matrix<T> Matrix<T>::MakeRotation(T angle, Vec3<T> rotation) {
	T cTheta = cosf(angle);
	T sTheta = sinf(angle);
	T oneMinCTheta = T(1) - cosf(angle);
	T x = rotation.x;
	T y = rotation.y;
	T z = rotation.z;

    Matrix<T> res;
    res.m[0][0] = x*x*oneMinCTheta + cTheta;
    res.m[1][0] = x*y*oneMinCTheta - z*sTheta;
    res.m[2][0] = x*z*oneMinCTheta + y*sTheta;
    res.m[3][0] = T(0.0);

    res.m[0][1] = x*y*oneMinCTheta + z*sTheta;
    res.m[1][1] = y*y*oneMinCTheta + cTheta;
    res.m[2][1] = y*z*oneMinCTheta - x*sTheta;
    res.m[3][1] = T(0.0);

    res.m[0][2] = x*z*oneMinCTheta - y*sTheta;
    res.m[1][2] = y*z*oneMinCTheta + x*sTheta;
    res.m[2][2] = z*z*oneMinCTheta + cTheta;
    res.m[3][2] = T(0.0);

    res.m[0][3] = T(0.0);
    res.m[1][3] = T(0.0);
    res.m[2][3] = T(0.0);
    res.m[3][3] = T(1.0);
    return res;
}

// ***********************************************************************

template<typename T>
inline Matrix<T> Matrix<T>::MakeScale(Vec3<T> scale) {
    Matrix<T> mat;
    mat.m[0][0] = scale.x;
    mat.m[1][0] = T(0.0);
    mat.m[2][0] = T(0.0);
    mat.m[3][0] = T(0.0);

    mat.m[0][1] = T(0.0);
    mat.m[1][1] = scale.y;
    mat.m[2][1] = T(0.0);
    mat.m[3][1] = T(0.0);

    mat.m[0][2] = T(0.0);
    mat.m[1][2] = T(0.0);
    mat.m[2][2] = scale.z;
    mat.m[3][2] = T(0.0);

    mat.m[0][3] = T(0.0);
    mat.m[1][3] = T(0.0);
    mat.m[2][3] = T(0.0);
    mat.m[3][3] = T(1.0);
    return mat;
}

// ***********************************************************************

template<typename T>
inline Matrix<T> Matrix<T>::Perspective(T screenWidth, T screenHeight, T nearPlane, T farPlane, T fov) {
    // @Improvement: Disallow nearplane 0
    T ar = screenWidth / screenHeight;
    T zRange = farPlane - nearPlane;
    T tanHalfFOV = tanf(ToRadian(fov * T(0.5)));

    Matrix<T> mat;
    mat.m[0][0] = T(1.0) / (tanHalfFOV * ar);
    mat.m[1][0] = T(0.0);
    mat.m[2][0] = T(0.0);
    mat.m[3][0] = T(0.0);

    mat.m[0][1] = T(0.0);
    mat.m[1][1] = T(1.0) / tanHalfFOV;
    mat.m[2][1] = T(0.0);
    mat.m[3][1] = T(0.0);

    mat.m[0][2] = T(0.0);
    mat.m[1][2] = T(0.0);
    mat.m[2][2] = -farPlane / zRange;
    mat.m[3][2] = -nearPlane * (farPlane / zRange);

    mat.m[0][3] = T(0.0);
    mat.m[1][3] = T(0.0);
    mat.m[2][3] = T(-1.0);
    mat.m[3][3] = T(0.0);
    return mat;
}

// ***********************************************************************

template<typename T>
inline Matrix<T> Matrix<T>::Orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 nearPlane, f32 farPlane) {
    Matrix<T> mat;
    mat.m[0][0] = T(2.0) / (right - left);
    mat.m[1][0] = T(0.0);
    mat.m[2][0] = T(0.0);
    mat.m[3][0] = (-right - left) / (right - left);

    mat.m[0][1] = T(0.0);
    mat.m[1][1] = T(2.0) / (top - bottom);
    mat.m[2][1] = T(0.0);
    mat.m[3][1] = (-top - bottom) / (top - bottom);

    mat.m[0][2] = T(0.0);
    mat.m[1][2] = T(0.0);
    mat.m[2][2] = T(-1.0) / (farPlane - nearPlane);
    mat.m[3][2] = (-nearPlane) / (farPlane - nearPlane);

    mat.m[0][3] = T(0.0);
    mat.m[1][3] = T(0.0);
    mat.m[2][3] = T(0.0);
    mat.m[3][3] = T(1.0);
    return mat;
}

// ***********************************************************************

template<typename T>
inline Matrix<T> Matrix<T>::MakeLookAt(Vec3<T> Forward, Vec3<T> Up) {
    Vec3<T> N = Forward.GetNormalized();
    Vec3<T> U = Up.GetNormalized();
    U = Vec3<T>::Cross(U, N);
    Vec3<T> V = Vec3<T>::Cross(N, U);

    Matrix<T> mat;
    mat.m[0][0] = U.x;
    mat.m[1][0] = U.y;
    mat.m[2][0] = U.z;
    mat.m[3][0] = T(0.0);

    mat.m[0][1] = V.x;
    mat.m[1][1] = V.y;
    mat.m[2][1] = V.z;
    mat.m[3][1] = T(0.0);

    mat.m[0][2] = N.x;
    mat.m[1][2] = N.y;
    mat.m[2][2] = N.z;
    mat.m[3][2] = T(0.0);

    mat.m[0][3] = T(0.0);
    mat.m[1][3] = T(0.0);
    mat.m[2][3] = T(0.0);
    mat.m[3][3] = T(1.0);
    return mat;
}
