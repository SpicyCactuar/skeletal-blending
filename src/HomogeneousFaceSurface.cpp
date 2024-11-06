#include "HomogeneousFaceSurface.h"

#include <iomanip>
#include <fstream>
#include <cmath>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#endif

HomogeneousFaceSurface::HomogeneousFaceSurface() {
    vertices.clear();
    normals.clear();
}

bool HomogeneousFaceSurface::readTriangleSoupFile(const char* fileName) {
    std::ifstream inFile(fileName);
    if (inFile.bad()) {
        return false;
    }

    // Read in number of triangles and, consequently, vertices
    long nTriangles = 0, nVertices = 0;
    inFile >> nTriangles;
    nVertices = nTriangles * 3;

    // Parse all triangles
    vertices.resize(nVertices);
    for (int vertex = 0; vertex < nVertices; vertex++) {
        inFile >> vertices[vertex].x >> vertices[vertex].y >> vertices[vertex].z;
        // Vertices have v.w = 1.0
        vertices[vertex].w = 1.0;
    }

    computeUnitNormalVectors();

    return true;
}

void HomogeneousFaceSurface::computeUnitNormalVectors() {
    // assume that the triangle vertices are set correctly, and allocate one third of that for normals
    normals.resize(vertices.size() / 3);

    // loop through the triangles, computing normal vectors
    for (size_t triangle = 0; triangle < normals.size(); triangle++) {
        const Cartesian3 p = vertices[3 * triangle].Point();
        const Cartesian3 q = vertices[3 * triangle + 1].Point();
        const Cartesian3 r = vertices[3 * triangle + 2].Point();

        // compute two edge vectors
        const Cartesian3 u = q - p;
        const Cartesian3 v = r - p;

        // compute a normal with the cross-product
        const Cartesian3 normal = u.cross(v).unit();

        normals[triangle] = Homogeneous4(normal.x, normal.y, normal.z, 0.0);
    }
}


void HomogeneousFaceSurface::render(const Matrix4& viewMatrix) const {
    glBegin(GL_TRIANGLES);

    // we loop through all of the triangles
    for (size_t triangle = 0; triangle < normals.size(); triangle++) {
        Homogeneous4 p = viewMatrix * vertices[3 * triangle];
        Homogeneous4 q = viewMatrix * vertices[3 * triangle + 1];
        Homogeneous4 r = viewMatrix * vertices[3 * triangle + 2];
        Homogeneous4 normal = viewMatrix * normals[triangle];

        // this works because C++ guarantees that the POD data is in exactly
        // the order stated in the class with no padding.
        glNormal3fv(&normal.x);
        glVertex4fv(&p.x);
        glVertex4fv(&q.x);
        glVertex4fv(&r.x);
    }

    glEnd();
}
