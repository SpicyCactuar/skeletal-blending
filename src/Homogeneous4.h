#ifndef HOMOGENEOUS4_H
#define HOMOGENEOUS4_H

#include <iostream>

#include "Cartesian3.h"

class Homogeneous4 {
public:
    // we rely on POD for sending to GPU
    float x, y, z, w;

    Homogeneous4();

    Homogeneous4(float x, float y, float z, float w = 1.0);

    Homogeneous4(const Cartesian3& other);

    // point resulting from perspective division
    Cartesian3 Point() const;

    // point resulting from dropping w
    Cartesian3 Vector() const;

    float& operator [](int index);

    const float& operator [](int index) const;
};

std::ostream& operator <<(std::ostream& outStream, const Homogeneous4& value);

#endif
