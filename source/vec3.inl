#include "vec3.h"

#include <math.h>

#include "log.h"

// ***********************************************************************

template<typename T>
inline Vec3<T> Vec3<T>::operator+(const T& scalar) const {
    return Vec3(x + scalar, y + scalar, z + scalar);
}

// ***********************************************************************

template<typename T>
inline Vec3<T> Vec3<T>::operator-(const T& scalar) const {
    return Vec3(x - scalar, y - scalar, z - scalar);
}

// ***********************************************************************

template<typename T>
inline Vec3<T> Vec3<T>::operator*(const T& scalar) const {
    return Vec3(x * scalar, y * scalar, z * scalar);
}

// ***********************************************************************

template<typename T>
inline Vec3<T> Vec3<T>::operator/(const T& scalar) const {
    return Vec3(x / scalar, y / scalar, z / scalar);
}

// ***********************************************************************

template<typename T>
inline Vec3<T> Vec3<T>::operator+(const Vec3& rhs) const {
    return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
}

// ***********************************************************************

template<typename T>
inline Vec3<T> Vec3<T>::operator-(const Vec3& rhs) const {
    return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
}

// ***********************************************************************

template<typename T>
inline Vec3<T> Vec3<T>::operator-() const {
    return Vec3(-x, -y, -z);
}

// ***********************************************************************

template<typename T>
inline static Vec3<T> Vec3<T>::CompMul(const Vec3& lhs, const Vec3& rhs) {
    return Vec3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
}

// ***********************************************************************

template<typename T>
inline static Vec3<T> Vec3<T>::CompDiv(const Vec3& lhs, const Vec3& rhs) {
    return Vec3(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
}

// ***********************************************************************

template<typename T>
inline static T Vec3<T>::Dot(const Vec3& lhs, const Vec3& rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

// ***********************************************************************

template<typename T>
inline Vec3<T> Vec3<T>::Cross(const Vec3& lhs, const Vec3& rhs) {
    return Vec3(
        lhs.y * rhs.z - lhs.z * rhs.y,
        lhs.z * rhs.x - lhs.x * rhs.z,
        lhs.x * rhs.y - lhs.y * rhs.x);
}

// ***********************************************************************

template<typename T>
inline bool Vec3<T>::IsEquivalent(const Vec3& lhs, const Vec3& rhs) {
    return (lhs - rhs).GetLengthSquared() < 0.003f;
}

// ***********************************************************************

template<typename T>
inline bool Vec3<T>::operator==(const Vec3& rhs) const {
    return x == rhs.x && y == rhs.y && z == rhs.z;
}

// ***********************************************************************

template<typename T>
inline bool Vec3<T>::operator!=(const Vec3& rhs) const {
    return x != rhs.x && y != rhs.y && z != rhs.z;
}

// ***********************************************************************

template<typename T>
inline Vec3<T> Vec3<T>::operator+=(const T& scalar) {
    x += scalar;
    y += scalar;
    z += scalar;
    return *this;
}

// ***********************************************************************

template<typename T>
inline Vec3<T> Vec3<T>::operator-=(const T& scalar) {
    x -= scalar;
    y -= scalar;
    z -= scalar;
    return *this;
}

// ***********************************************************************

template<typename T>
inline Vec3<T> Vec3<T>::operator*=(const T& scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

// ***********************************************************************

template<typename T>
inline Vec3<T> Vec3<T>::operator/=(const T& scalar) {
    x /= scalar;
    y /= scalar;
    z /= scalar;
    return *this;
}

// ***********************************************************************

template<typename T>
inline Vec3<T> Vec3<T>::operator+=(const Vec3& rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

// ***********************************************************************

template<typename T>
inline Vec3<T> Vec3<T>::operator-=(const Vec3& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
}

// ***********************************************************************

template<typename T>
inline T Vec3<T>::GetLength() const {
    return sqrtf(x * x + y * y + z * z);
}

// ***********************************************************************

template<typename T>
inline T Vec3<T>::GetLengthSquared() const {
    return x * x + y * y + z * z;
}

// ***********************************************************************

template<typename T>
inline Vec3<T> Vec3<T>::GetNormalized() const {
    return Vec3(x, y, z) / GetLength();
}

// ***********************************************************************

template<typename T>
inline T& Vec3<T>::operator[](int index) {
    AssertMsg(index < 3, "Out of bounds index for vector component");
    return (&x)[index];
}

// ***********************************************************************

template<typename T>
inline T Vec3<T>::operator[](int index) const {
    AssertMsg(index < 3, "Out of bounds index for vector component");
    return (&x)[index];
}

// ***********************************************************************

template<typename T>
inline Vec3<T> Vec3<T>::Project4D(const Vec4<T>& vec) {
    return Vec3(vec.x, vec.y, vec.z);
}

// ***********************************************************************

template<typename T>
inline Vec3<T> Vec3<T>::Embed2D(const Vec2<T>& vec, T _z) {
    return Vec3(vec.x, vec.y, _z);
}