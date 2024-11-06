#include "Homogeneous4.h"

#include <iomanip>

Homogeneous4::Homogeneous4()
    : x(0.0f),
      y(0.0f),
      z(0.0f),
      w(0.0f) {
}

Homogeneous4::Homogeneous4(const float x, const float y, const float z, const float w)
    : x(x),
      y(y),
      z(z),
      w(w) {
}

Homogeneous4::Homogeneous4(const Cartesian3& other)
    : x(other.x),
      y(other.y),
      z(other.z),
      w(1.0f) {
}

Cartesian3 Homogeneous4::Point() const {
    return Cartesian3(x / w, y / w, z / w);
}

Cartesian3 Homogeneous4::Vector() const {
    return Cartesian3(x, y, z);
}

float& Homogeneous4::operator [](const int index) {
    switch (index) {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        case 3:
            return w;
        // error case, choose to default to x
        default:
            return x;
    }
}

const float& Homogeneous4::operator [](const int index) const {
    switch (index) {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        case 3:
            return w;
        default:
            // error case, choose to default to x
                return x;
    }
}

std::ostream& operator <<(std::ostream& outStream, const Homogeneous4& value) {
    return outStream << std::setprecision(4) << value.x << " " << value.y << " " << value.z << " " << value.w;
}
