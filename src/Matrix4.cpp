#include "Matrix4.h"

#include <iomanip>
#include <cmath>

Matrix4::Matrix4() {
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            coordinates[row][col] = 0.0f;
        }
    }
}

float* Matrix4::operator [](const int rowIndex) {
    return coordinates[rowIndex];
}

const float* Matrix4::operator [](const int rowIndex) const {
    return coordinates[rowIndex];
}

Matrix4 Matrix4::operator *(const float factor) const {
    Matrix4 result;

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            result.coordinates[row][col] = coordinates[row][col] * factor;
        }
    }

    return result;
}

Homogeneous4 Matrix4::operator *(const Homogeneous4& vector) const {
    Homogeneous4 result;

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            result[row] += coordinates[row][col] * vector[col];
        }
    }

    return result;
}

Cartesian3 Matrix4::operator *(const Cartesian3& vector) const {
    return (*this * Homogeneous4(vector)).Point();
}

Matrix4 Matrix4::operator *(const Matrix4& other) const {
    Matrix4 result;

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            for (int entry = 0; entry < 4; entry++) {
                result.coordinates[row][col] += coordinates[row][entry] * other.coordinates[entry][col];
            }
        }
    }

    return result;
}

Matrix4 Matrix4::transpose() const {
    Matrix4 result;

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            result.coordinates[row][col] = coordinates[col][row];
        }
    }

    return result;
}

Matrix4 Matrix4::identity() {
    Matrix4 returnMatrix;

    // fill in the diagonal with 1.0f
    for (int row = 0; row < 4; row++) {
        returnMatrix.coordinates[row][row] = 1.0;
    }

    return returnMatrix;
}

Matrix4 Matrix4::translation(const Cartesian3& vector) {
    // Start with identity
    Matrix4 result = identity();

    // put the translation in the w column
    for (int entry = 0; entry < 3; entry++) {
        result.coordinates[entry][3] = vector[entry];
    }

    return result;
}

Matrix4 Matrix4::rotationX(const float degrees) {
    // convert angle from degrees to radians
    const float theta = DEG2RAD(degrees);

    Matrix4 result = identity();

    // set only the four coefficients affected
    result.coordinates[1][1] = std::cos(theta);
    result.coordinates[1][2] = std::sin(theta);
    result.coordinates[2][1] = -std::sin(theta);
    result.coordinates[2][2] = std::cos(theta);

    return result;
}

Matrix4 Matrix4::rotationY(const float degrees) {
    // convert angle from degrees to radians
    const float theta = DEG2RAD(degrees);

    Matrix4 result = identity();

    // set only the four coefficients affected
    result.coordinates[0][0] = std::cos(theta);
    result.coordinates[0][2] = -std::sin(theta);
    result.coordinates[2][0] = std::sin(theta);
    result.coordinates[2][2] = std::cos(theta);

    return result;
}

Matrix4 Matrix4::rotationZ(const float degrees) {
    // convert angle from degrees to radians
    const float theta = DEG2RAD(degrees);

    Matrix4 result = identity();

    // set only the four coefficients affected
    result.coordinates[0][0] = std::cos(theta);
    result.coordinates[0][1] = std::sin(theta);
    result.coordinates[1][0] = -std::sin(theta);
    result.coordinates[1][1] = std::cos(theta);

    return result;
}

Matrix4 Matrix4::rotateBetween(const Cartesian3& vector1, const Cartesian3& vector2) {
    const Cartesian3 cross = vector1.cross(vector2).unit();
    const float cos = vector1.unit().dot(vector2.unit());
    const float sin = std::sqrt(1 - pow(cos, 2));

    Matrix4 result = identity();
    result.coordinates[0][0] = cos + (1 - cos) * pow(cross.x, 2);
    result.coordinates[0][1] = (1 - cos) * cross.x * cross.y - sin * cross.z;
    result.coordinates[0][2] = (1 - cos) * cross.x * cross.z + sin * cross.y;
    result.coordinates[1][0] = (1 - cos) * cross.y * cross.x + sin * cross.z;
    result.coordinates[1][1] = cos + (1 - cos) * pow(cross.y, 2);
    result.coordinates[1][2] = (1 - cos) * cross.y * cross.z - sin * cross.x;
    result.coordinates[2][0] = (1 - cos) * cross.z * cross.x - sin * cross.y;
    result.coordinates[2][1] = (1 - cos) * cross.z * cross.y + sin * cross.x;
    result.coordinates[2][2] = cos + (1 - cos) * pow(cross.z, 2);
    return result;
}

std::ostream& operator <<(std::ostream& outStream, const Matrix4& value) {
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            outStream << std::setw(12)
                    << std::setprecision(5)
                    << std::fixed << value.coordinates[row][col]
                    << (col == 3)
                        ? "\n"
                        : " ";
        }
    }
    return outStream;
}
