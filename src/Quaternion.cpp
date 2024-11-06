#include "Quaternion.h"

#include <cmath>

Quaternion::Quaternion() {
    q[0] = q[1] = q[2] = 0.0;
    q[3] = 1.0;
}

Quaternion::Quaternion(const Cartesian3& axis, float theta) {
    const float thetaRad = M_PI * theta / 180.0f;
    Cartesian3 v = std::sin(thetaRad) * axis.unit();
    q[0] = v[0];
    q[1] = v[1];
    q[2] = v[2];

    q[3] = std::cos(thetaRad);
}

Matrix4 Quaternion::matrix() const {
    Matrix4 result;
    /**
     * A Quaternion (x y z w) is equivalent to the following rotation matrix:
     *
     * | 1 - 2(y^2+z^2)          2(xy-wz)          2(xz+wy)    0 |
     * |       2(xy+wz)    1 - 2(x^2+z^2)          2(yz-wx)    0 |
     * |       2(xz-wy)          2(yz+wx)    1 - 2(x^2+y^2)    0 |
     * |              0                 0                 0    1 |
     *
     * This matrix is obtained by extracting the pre and post-multiplication matrices
     * of the action of the Quaternion (q * p * q^(-1)) and multiplying them.
     * Note that a unit Quaternion must be used, otherwise the resulting matrix might not
     * be a pure rotation matrix.
     */
    float xx = q[0] * q[0];
    float xy = q[0] * q[1];
    float xz = q[0] * q[2];
    float xw = q[0] * q[3];

    float yy = q[1] * q[1];
    float yz = q[1] * q[2];
    float yw = q[1] * q[3];

    float zz = q[2] * q[2];
    float zw = q[2] * q[3];

    result.coordinates[0][0] = 1 - 2 * (yy + zz);
    result.coordinates[0][1] = 2 * (xy - zw);
    result.coordinates[0][2] = 2 * (xz + yw);
    result.coordinates[0][3] = 0.0;

    result.coordinates[1][0] = 2 * (xy + zw);
    result.coordinates[1][1] = 1 - 2 * (xx + zz);
    result.coordinates[1][2] = 2 * (yz - xw);
    result.coordinates[1][3] = 0.0;

    result.coordinates[2][0] = 2 * (xz - yw);
    result.coordinates[2][1] = 2 * (yz + xw);
    result.coordinates[2][2] = 1 - 2 * (xx + yy);
    result.coordinates[2][3] = 0.0;

    result.coordinates[3][0] = 0.0;
    result.coordinates[3][1] = 0.0;
    result.coordinates[3][2] = 0.0;
    result.coordinates[3][3] = 1.0;

    return result;
}

Quaternion Quaternion::operator+(const Quaternion& other) const {
    Quaternion result;
    for (int i = 0; i < 4; i++) {
        result.q[i] = q[i] + other.q[i];
    }
    return result;
}

Quaternion Quaternion::operator*(const Quaternion& other) const {
    Quaternion result;

    // i term
    result.q[0] = q[0] * other.q[3] // i * 1 = i
                  + q[1] * other.q[2] // j * k = i
                  - q[2] * other.q[1] // k * j = (-i)
                  + q[3] * other.q[0]; // 1 * i = i

    // j term
    result.q[1] = -q[0] * other.q[2] // i * k = (-j)
                  + q[1] * other.q[3] // j * 1 = j
                  + q[2] * other.q[0] // k * i = j
                  + q[3] * other.q[1]; // 1 * j = j

    // k term
    result.q[2] = q[0] * other.q[1] // i * j = k
                  - q[1] * other.q[0] // j * i = (-k)
                  + q[2] * other.q[3] // k * 1 = k
                  + q[3] * other.q[2]; // 1 * k = k

    // Real term
    result.q[3] = -q[0] * other.q[0] // i * i = (-1)
                  - q[1] * other.q[1] // j * j = (-1)
                  - q[2] * other.q[2] // k * k = (-1)
                  + q[3] * other.q[3]; // 1 * 1 = 1

    return result;
}

float Quaternion::dot(const Quaternion& other) const {
    return q[0] * other.q[0] + q[1] * other.q[1] + q[2] * other.q[2] + q[3] * other.q[3];
}

Quaternion operator*(float scalar, const Quaternion& other) {
    Quaternion result;
    for (int i = 0; i < 4; i++) {
        result.q[i] = scalar * other.q[i];
    }
    return result;
}

std::ostream& operator<<(std::ostream& outStream, const Quaternion& quaternion) {
    return outStream
           << quaternion.q[0] << " "
           << quaternion.q[1] << " "
           << quaternion.q[2] << " "
           << quaternion.q[3] << std::endl;
}

/**
 * Assumptions:
 *      - q0 and q1 are unit Quaternions
 *      - t in [0..1]
 */
Quaternion slerp(const Quaternion& q0, const Quaternion& q1, const float t) {
    const float cosTheta = q0.dot(q1);

    // LERP when Quaternions are (close to) parallel
    // Avoids SLERP division by 0
    if (cosTheta - 1.0f < std::numeric_limits<float>::epsilon()) {
        return (1.0f - t) * q0 + t * q1;
    }

    float angle = std::acos(cosTheta);

    float d = std::sin(angle);
    float s0 = std::sin((1.0f - t) * angle) / d;
    float s1 = std::sin(t * angle) / d;

    return s0 * q0 + s1 * q1;
}
