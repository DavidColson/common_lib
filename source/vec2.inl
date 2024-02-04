#include "vec2.h"

// ***********************************************************************

template<typename T>
inline Vec2<T> Vec2<T>::operator+(const T& scalar) const {
    return Vec2(x + scalar, y + scalar);
}

// ***********************************************************************

template<typename T>
inline Vec2<T> Vec2<T>::operator-(const T& scalar) const {
    return Vec2(x - scalar, y - scalar);
}

// ***********************************************************************

template<typename T>
inline Vec2<T> Vec2<T>::operator*(const T& scalar) const {
    return Vec2(x * scalar, y * scalar);
}

// ***********************************************************************

template<typename T>
inline Vec2<T> Vec2<T>::operator/(const T& scalar) const {
    return Vec2(x / scalar, y / scalar);
}

// ***********************************************************************

template<typename T>
inline Vec2<T> Vec2<T>::operator+(const Vec2& rhs) const {
    return Vec2(x + rhs.x, y + rhs.y);
}

// ***********************************************************************

template<typename T>
inline Vec2<T> Vec2<T>::operator-(const Vec2& rhs) const {
    return Vec2(x - rhs.x, y - rhs.y);
}

// ***********************************************************************

template<typename T>
inline Vec2<T> Vec2<T>::operator-() const {
    return Vec2(-x, -y);
}

// ***********************************************************************

template<typename T>
inline Vec2<T> Vec2<T>::CompMul(const Vec2& lhs, const Vec2& rhs) {
    return Vec2(lhs.x * rhs.x, lhs.y * rhs.y);
}

// ***********************************************************************

template<typename T>
inline Vec2<T> Vec2<T>::CompDiv(const Vec2& lhs, const Vec2& rhs) {
    return Vec2(lhs.x / rhs.x, lhs.y / rhs.y);
}

// ***********************************************************************

template<typename T>
inline T Vec2<T>::Dot(const Vec2& lhs, const Vec2& rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

// ***********************************************************************

template<typename T>
inline f32 Vec2<T>::Cross(const Vec2& lhs, const Vec2& rhs) {
    return lhs.x * rhs.y - lhs.y * rhs.x;
}

// ***********************************************************************

template<typename T>
inline bool Vec2<T>::operator==(const Vec2& rhs) const {
    return x == rhs.x && y == rhs.y;
}

// ***********************************************************************

template<typename T>
inline bool Vec2<T>::operator!=(const Vec2& rhs) const {
    return x != rhs.x || y != rhs.y;
}

// ***********************************************************************

template<typename T>
inline Vec2<T> Vec2<T>::operator+=(const T& scalar) {
    x += scalar;
    y += scalar;
    return *this;
}

// ***********************************************************************

template<typename T>
inline Vec2<T> Vec2<T>::operator-=(const T& scalar) {
    x -= scalar;
    y -= scalar;
    return *this;
}

// ***********************************************************************

template<typename T>
inline Vec2<T> Vec2<T>::operator*=(const T& scalar) {
    x *= scalar;
    y *= scalar;
    return *this;
}

// ***********************************************************************

template<typename T>
inline Vec2<T> Vec2<T>::operator/=(const T& scalar) {
    x /= scalar;
    y /= scalar;
    return *this;
}

// ***********************************************************************

template<typename T>
inline Vec2<T> Vec2<T>::operator+=(const Vec2& rhs) {
    x += rhs.x;
    y += rhs.y;
    return *this;
}

// ***********************************************************************

template<typename T>
inline Vec2<T> Vec2<T>::operator-=(const Vec2& rhs) {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

// ***********************************************************************

template<typename T>
inline T Vec2<T>::GetLength() const {
    return (T)sqrt(x * x + y * y);
}

// ***********************************************************************

template<typename T>
inline Vec2<T> Vec2<T>::GetNormalized() const {
    return Vec2(x, y) / GetLength();
}

// ***********************************************************************

template<typename T>
inline T& Vec2<T>::operator[](int index) {
    // ASSERT(index < 2, "Out of bounds index for vector component");
    return (&x)[index];
}

// ***********************************************************************

template<typename T>
inline T Vec2<T>::operator[](int index) const {
    // ASSERT(index < 2, "Out of bounds index for vector component");
    return (&x)[index];
}

// ***********************************************************************

template<typename T>
inline Vec2<T> Vec2<T>::Project4D(const Vec4<T>& vec) {
    return Vec2(vec.x, vec.y);
}

// ***********************************************************************

template<typename T>
inline Vec2<T> Vec2<T>::Project3D(const Vec3<T>& vec) {
    return Vec2(vec.x, vec.y);
}
