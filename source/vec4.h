// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

#include "types.h"

template<typename T>
struct Vec3;

template<typename T>
struct Vec2;

template<typename T>
struct Vec4 {
    Vec4()
        : x(T()), y(T()), z(T()), w(T()) {}
    Vec4(T val)
        : x(val), y(val), z(val), w(val) {}
    Vec4(T x, T y, T z, T w)
        : x(x), y(y), z(z), w(w) {}

    T x;
    T y;
    T z;
    T w;

    /**
     * Add a scalar to each component of this vector and return the result
     *
     * @param  scalar The scalar to add
     * @return The result of adding the scalar
     **/
    inline Vec4 operator+(const T& scalar) const;

    /**
     * Subtract a scalar from this vector and return the result
     *
     * @param  scalar The scalar to subtract
     * @return The result of subtracting the scalar
     **/
    inline Vec4 operator-(const T& scalar) const;

    /**
     * Multiply each component by this vector and return the result
     *
     * @param  scalar The scalar to multiply
     * @return The result of multiplying by the scalar
     **/
    inline Vec4 operator*(const T& scalar) const;

    /**
     * Divide each component by a scalar and return the result
     *
     * @param  scalar The scalar to divide with
     * @return The result of dividing the vector
     **/
    inline Vec4 operator/(const T& scalar) const;

    /**
     * Add a vector to this vector and return the result
     *
     * @param  rhs The vector to add to this
     * @return The result of adding the two vectors
     **/
    inline Vec4 operator+(const Vec4& rhs) const;

    /**
     * Subtract a vector from this vector and return the result
     *
     * @param  rhs The vector to take away from this
     * @return The result of the subtraction
     **/
    inline Vec4 operator-(const Vec4& rhs) const;

    /**
     * Negate this vector and return the result
     *
     * @return The negated vector
     **/
    inline Vec4 operator-() const;

    /**
     * Component multiply two vectors and return the result
     *
     * @param  lhs The left vector
     * @param  rhs The right vector
     * @return The result of the component multiplication
     **/
    inline static Vec4 CompMul(const Vec4& lhs, const Vec4& rhs);

    /**
     * Component divide two vectors and return the result
     *
     * @param  lhs The left vector
     * @param  rhs The right vector
     * @return The result of the component division
     **/
    inline static Vec4 CompDiv(const Vec4& lhs, const Vec4& rhs);

    /**
     * Take the 3D dot product of two 4D vectors and return the result
     *
     * @param  lhs The left vector
     * @param  rhs The right vector
     * @return The dot of the two vectors
     **/
    inline static T Dot3(const Vec4& lhs, const Vec4& rhs);

    /**
     * Take the 4D dot product of two 4D vectors and return the result
     *
     * @param  lhs The left vector
     * @param  rhs The right vector
     * @return The dot of the two vectors
     **/
    inline static T Dot(const Vec4& lhs, const Vec4& rhs);

    /**
     * Calculate the 3D cross product of two vectors (the w component is ignored).
     *
     * @param  lhs The left vector
     * @param  rhs The right vector
     * @return The cross product of the two vectors
     **/
    inline static Vec4 Cross(const Vec4& lhs, const Vec4& rhs);

    /**
     * Compare two vectors for exact equality
     *
     * @param  rhs The other vector to compare against
     * @return True if the vectors are exactly equal
     **/
    inline bool operator==(const Vec4& rhs) const;

    /**
     * Compare two vectors for inequality
     *
     * @param  rhs The other vector to compare against
     * @return True if the vectors are not equal
     **/
    inline bool operator!=(const Vec4& rhs) const;

    /**
     * Add a scalar to this vector, modifying this vector
     *
     * @param  scalar The scalar to add
     * @return This vector after the addition
     **/
    inline Vec4 operator+=(const T& scalar);

    /**
     * Subtract a scalar to this vector, modifying this vector
     *
     * @param  scalar The scalar to subtract
     * @return This vector after the subtraction
     **/
    inline Vec4 operator-=(const T& scalar);

    /**
     * Multiply this vector by a scalar, modifying this vector
     *
     * @param  scalar The scalar to multiply by
     * @return This vector after the multiplication
     **/
    inline Vec4 operator*=(const T& scalar);

    /**
     * Divide this vector by a scalar, modifying this vector
     *
     * @param  scalar The scalar to divide by
     * @return This vector after the division
     **/
    inline Vec4 operator/=(const T& scalar);

    /**
     * Add another vector to this vector, modifying this vector
     *
     * @param  rhs The vector to to add
     * @return This vector after the addition
     **/
    inline Vec4 operator+=(const Vec4& rhs);

    /**
     * Subtract another vector from this vector, modifying this vector
     *
     * @param  rhs The vector to to subtract
     * @return This vector after the subtraction
     **/
    inline Vec4 operator-=(const Vec4& rhs);

    /**
     * Get the length of this vector
     *
     * @return The length
     **/
    inline T GetLength() const;

    /**
     * Get a normalized version of this vector
     *
     * @return The normalized vector
     **/
    inline Vec4 GetNormalized() const;

    /**
     * Get a reference to an element of this vector by index
     *
     * @param  index The index to query
     * @return A reference to that element
     **/
    inline T& operator[](int index);

    /**
     * Get a copy of an element of this vector by index
     *
     * @param  index The index to query
     * @return A copy of that element
     **/
    inline T operator[](int index) const;


    /**
     * Embeds a 3D vector into 4D space
     *
     * @param  vec The target 3D vector
     * @return The 4D vector
     **/
    inline static Vec4 Embed3D(const Vec3<T>& vec);

    /**
     * Embeds a 2D vector into 4D space
     *
     * @param  vec The target 3D vector
     * @return The 4D vector
     **/
    inline static Vec4 Embed2D(const Vec2<T>& vec);
};

typedef Vec4<f32> Vec4f;
typedef Vec4<f64> Vec4d;
