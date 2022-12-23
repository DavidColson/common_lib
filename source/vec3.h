// Copyright 2020-2022 David Colson. All rights reserved.

#pragma once

template<typename T>
struct Vec4;

template<typename T>
struct Vec2;

template<typename T>
struct Vec3 {
    Vec3()
        : x(T()), y(T()), z(T()) {}
    Vec3(T val)
        : x(val), y(val), z(val) {}
    Vec3(T x, T y, T z)
        : x(x), y(y), z(z) {}

    T x;
    T y;
    T z;

    /**
     * Add a scalar to each component of this vector and return the result
     *
     * @param  scalar The scalar to add
     * @return The result of adding the scalar
     **/
    inline Vec3 operator+(const T& scalar) const;

    /**
     * Subtract a scalar from this vector and return the result
     *
     * @param  scalar The scalar to subtract
     * @return The result of subtracting the scalar
     **/
    inline Vec3 operator-(const T& scalar) const;

    /**
     * Multiply each component by this vector and return the result
     *
     * @param  scalar The scalar to multiply
     * @return The result of multiplying by the scalar
     **/
    inline Vec3 operator*(const T& scalar) const;

    /**
     * Divide each component by a scalar and return the result
     *
     * @param  scalar The scalar to divide with
     * @return The result of dividing the vector
     **/
    inline Vec3 operator/(const T& scalar) const;

    /**
     * Add a vector to this vector and return the result
     *
     * @param  rhs The vector to add to this
     * @return The result of adding the two vectors
     **/
    inline Vec3 operator+(const Vec3& rhs) const;

    /**
     * Subtract a vector from this vector and return the result
     *
     * @param  rhs The vector to take away from this
     * @return The result of the subtraction
     **/
    inline Vec3 operator-(const Vec3& rhs) const;

    /**
     * Negate this vector and return the result
     *
     * @return The negated vector
     **/
    inline Vec3 operator-() const;

    /**
     * Component multiply two vectors and return the result
     *
     * @param  lhs The left vector
     * @param  rhs The right vector
     * @return The result of the component multiplication
     **/
    inline static Vec3 CompMul(const Vec3& lhs, const Vec3& rhs);

    /**
     * Component divide two vectors and return the result
     *
     * @param  lhs The left vector
     * @param  rhs The right vector
     * @return The result of the component division
     **/
    inline static Vec3 CompDiv(const Vec3& lhs, const Vec3& rhs);

    /**
     * Take the dot product of two vectors and return the result
     *
     * @param  lhs The left vector
     * @param  rhs The right vector
     * @return The dot of the two vectors
     **/
    inline static T Dot(const Vec3& lhs, const Vec3& rhs);

    /**
     * Calculate the cross product of two vectors.
     *
     * @param  lhs The left vector
     * @param  rhs The right vector
     * @return The cross product of the two vectors
     **/
    inline static Vec3 Cross(const Vec3& lhs, const Vec3& rhs);

    /**
     * Determine if two vectors are almost the same
     *
     * @param  lhs The left vector
     * @param  rhs The right vector
     * @return true or false, are they equivalent
     **/
    inline static bool IsEquivalent(const Vec3& lhs, const Vec3& rhs);

    /**
     * Compare two vectors for exact equality
     *
     * @param  rhs The other vector to compare against
     * @return True if the vectors are exactly equal
     **/
    inline bool operator==(const Vec3& rhs) const;

    /**
     * Compare two vectors for inequality
     *
     * @param  rhs The other vector to compare against
     * @return True if the vectors are not equal
     **/
    inline bool operator!=(const Vec3& rhs) const;

    /**
     * Add a scalar to this vector, modifying this vector
     *
     * @param  scalar The scalar to add
     * @return This vector after the addition
     **/
    inline Vec3 operator+=(const T& scalar);

    /**
     * Subtract a scalar to this vector, modifying this vector
     *
     * @param  scalar The scalar to subtract
     * @return This vector after the subtraction
     **/
    inline Vec3 operator-=(const T& scalar);

    /**
     * Multiply this vector by a scalar, modifying this vector
     *
     * @param  scalar The scalar to multiply by
     * @return This vector after the multiplication
     **/
    inline Vec3 operator*=(const T& scalar);

    /**
     * Divide this vector by a scalar, modifying this vector
     *
     * @param  scalar The scalar to divide by
     * @return This vector after the division
     **/
    inline Vec3 operator/=(const T& scalar);

    /**
     * Add another vector to this vector, modifying this vector
     *
     * @param  rhs The vector to to add
     * @return This vector after the addition
     **/
    inline Vec3 operator+=(const Vec3& rhs);

    /**
     * Subtract another vector from this vector, modifying this vector
     *
     * @param  rhs The vector to to subtract
     * @return This vector after the subtraction
     **/
    inline Vec3 operator-=(const Vec3& rhs);

    /**
     * Get the length of this vector
     *
     * @return The length
     **/
    inline T GetLength() const;

    /**
     * Get the length of this vector squared
     *
     * @return The length squared
     **/
    inline T GetLengthSquared() const;

    /**
     * Get a normalized version of this vector
     *
     * @return The normalized vector
     **/
    inline Vec3 GetNormalized() const;

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
     * Projects a 4D vector into 3D space
     *
     * @param  vec The target 4D vector
     * @return The 3D vector
     **/
    inline static Vec3 Project4D(const Vec4<T>& vec);

    /**
     * Embeds a 2D vector into 3D space
     *
     * @param  vec The target 2D vector
     * @param  z the desired z component
     * @return The 3D vector
     **/
    inline static Vec3 Embed2D(const Vec2<T>& vec, T _z = T());
};

typedef Vec3<float> Vec3f;
typedef Vec3<double> Vec3d;
