#ifndef HOMOGENEOUS_FACE_SURFACE
#define HOMOGENEOUS_FACE_SURFACE

#include <vector>

#include "Homogeneous4.h"
#include "Matrix4.h"

class HomogeneousFaceSurface {
public:
    // Each trio of vertices forms a single triangle
    std::vector<Homogeneous4> vertices;

    // normals of the triangles
    std::vector<Homogeneous4> normals;

    HomogeneousFaceSurface();

    // reads .tri triangle soup file
    // returns true on success, false otherwise
    bool readTriangleSoupFile(const char* fileName);

    void computeUnitNormalVectors();

    void render(const Matrix4& viewMatrix) const;
};

#endif
