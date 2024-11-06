#ifndef CARTESIAN3_H
#define CARTESIAN3_H

#include <iostream>

class Cartesian3 {
public:
	// we rely on POD for sending to GPU
	float x, y, z;

	Cartesian3();

	Cartesian3(float x, float y, float z);

	Cartesian3 operator-() const;

	Cartesian3 operator +(const Cartesian3& other) const;

	Cartesian3 operator -(const Cartesian3& other) const;

	Cartesian3 operator *(float factor) const;

	Cartesian3 operator /(float factor) const;

	float dot(const Cartesian3& other) const;

	Cartesian3 cross(const Cartesian3& other) const;

	float length() const;

	Cartesian3 unit() const;

	float& operator [](int index);

	const float& operator [](int index) const;
};

Cartesian3 operator *(float factor, const Cartesian3& right);

std::istream& operator >>(std::istream& inStream, Cartesian3& value);

std::ostream& operator <<(std::ostream& outStream, const Cartesian3& value);

#endif
