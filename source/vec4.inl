#include "vec4.h"

// ***********************************************************************

template<typename T>
inline Vec4<T> Vec4<T>::operator+(const T& scalar) const {
    return Vec4(x + scalar, y + scalar, z + scalar, w + scalar);
}

// ***********************************************************************

template<typename T>
inline Vec4<T> Vec4<T>::operator-(const T& scalar) const {
    return Vec4(x - scalar, y - scalar, z - scalar, w - scalar);
}

// ***********************************************************************

template<typename T>
inline Vec4<T> Vec4<T>::operator*(const T& scalar) const {
    return Vec4(x * scalar, y * scalar, z * scalar, w * scalar);
}

// ***********************************************************************

template<typename T>
inline Vec4<T> Vec4<T>::operator/(const T& scalar) const {
    return Vec4(x / scalar, y / scalar, z / scalar, w / scalar);
}

// ***********************************************************************

template<typename T>
inline Vec4<T> Vec4<T>::operator+(const Vec4& rhs) const {
    return Vec4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
}

// ***********************************************************************

template<typename T>
inline Vec4<T> Vec4<T>::operator-(const Vec4& rhs) const {
    return Vec4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
}

// ***********************************************************************

template<typename T>
inline Vec4<T> Vec4<T>::operator-() const {
    return Vec4(-x, -y, -z, -w);
}

// ***********************************************************************

template<typename T>
inline Vec4<T> Vec4<T>::CompMul(const Vec4& lhs, const Vec4& rhs) {
    return Vec4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w);
}

// ***********************************************************************

template<typename T>
inline Vec4<T> Vec4<T>::CompDiv(const Vec4& lhs, const Vec4& rhs) {
    return Vec4(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w);
}

// ***********************************************************************

template<typename T>
inline T Vec4<T>::Dot3(const Vec4& lhs, const Vec4& rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

// ***********************************************************************

template<typename T>
inline T Vec4<T>::Dot(const Vec4& lhs, const Vec4& rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
}

// ***********************************************************************

template<typename T>
inline Vec4<T> Vec4<T>::Cross(const Vec4& lhs, const Vec4& rhs) {
    return Vec4(
        lhs.y * rhs.z - lhs.z * rhs.y,
        lhs.z * rhs.x - lhs.x * rhs.z,
        lhs.x * rhs.y - lhs.y * rhs.x,
        0.0f);
}

// ***********************************************************************

template<typename T>
inline bool Vec4<T>::operator==(const Vec4& rhs) const {
    return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
}

// ***********************************************************************

template<typename T>
inline bool Vec4<T>::operator!=(const Vec4& rhs) const {
    return x != rhs.x && y != rhs.y && z != rhs.z && w != rhs.w;
}

// ***********************************************************************

template<typename T>
inline Vec4<T> Vec4<T>::operator+=(const T& scalar) {
    x += scalar;
    y += scalar;
    z += scalar;
    w += scalar;
    return *this;
}

// ***********************************************************************

template<typename T>
inline Vec4<T> Vec4<T>::operator-=(const T& scalar) {
    x -= scalar;
    y -= scalar;
    z -= scalar;
    w -= scalar;
    return *this;
}

// ***********************************************************************

template<typename T>
inline Vec4<T> Vec4<T>::operator*=(const T& scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    w *= scalar;
    return *this;
}

// ***********************************************************************

template<typename T>
inline Vec4<T> Vec4<T>::operator/=(const T& scalar) {
    x /= scalar;
    y /= scalar;
    z /= scalar;
    w /= scalar;
    return *this;
}

// ***********************************************************************

template<typename T>
inline Vec4<T> Vec4<T>::operator+=(const Vec4& rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    w += rhs.w;
    return *this;
}

// ***********************************************************************

template<typename T>
inline Vec4<T> Vec4<T>::operator-=(const Vec4& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    w -= rhs.w;
    return *this;
}

// ***********************************************************************

template<typename T>
inline T Vec4<T>::GetLength() const {
    return sqrt(x * x + y * y + z * z + w * w);
}

// ***********************************************************************

template<typename T>
inline Vec4<T> Vec4<T>::GetNormalized() const {
    return Vec4(x, y, z, w) / GetLength();
}

// ***********************************************************************

template<typename T>
inline T& Vec4<T>::operator[](int index) {
    // ASSERT(index < 4, "Out of bounds index for vector component");
    return (&x)[index];
}

// ***********************************************************************

template<typename T>
inline T Vec4<T>::operator[](int index) const {
    // ASSERT(index < 4, "Out of bounds index for vector component");
    return (&x)[index];
}

// ***********************************************************************

template<typename T>
inline Vec4<T> Vec4<T>::Embed3D(const Vec3<T>& vec) {
    return Vec4(vec.x, vec.y, vec.z, 1.0f);
}

// ***********************************************************************

template<typename T>
inline Vec4<T> Vec4<T>::Embed2D(const Vec2<T>& vec) {
    return Vec4(vec.x, vec.y, 1.0f, 1.0f);
}