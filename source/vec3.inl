#include "vec3.h"

#include <math.h>

template<typename T>
inline T Vec3<T>::GetLength() const {
    return sqrtf(x * x + y * y + z * z);
}