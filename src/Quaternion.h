#ifndef QUATERNION
#define QUATERNION

#include "Cartesian3.h"
#include "Homogeneous4.h"
#include "Matrix4.h"

class Quaternion {
public:
    // The values (x, y, z, w) for the quaternion w + x*i + y*j + z*k
    Homogeneous4 q;

    // Quaternion with (x, y, z, w) = (0, 0, 0, 1) 
    Quaternion();

    // Quaternion that rotates (2 * theta) degrees around a given axis
    Quaternion(const Cartesian3& axis, float theta);

    // Returns corresponding rotation matrix
    Matrix4 matrix() const;

    Quaternion operator+(const Quaternion& other) const;

    Quaternion operator*(const Quaternion& other) const;

    float dot(const Quaternion& other) const;
};

Quaternion operator*(float scalar, const Quaternion& quat);

std::ostream& operator<<(std::ostream& outStream, const Quaternion& quat);

Quaternion slerp(const Quaternion& q0, const Quaternion& q1, float t);

#endif
