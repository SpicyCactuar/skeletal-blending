#ifndef TERRAIN
#define TERRAIN

#include <vector>

#include "HomogeneousFaceSurface.h"

class Terrain : public HomogeneousFaceSurface {
public:
    // height value per (x, y) coordinate
    std::vector<std::vector<float>> heightValues;
    float xyScale;

    Terrain();

    // reads .dem elevation/terrain model
    // xyScale gives the scale factor to use in the x-y directions
    bool readTerrainFile(const char* fileName, float xyScale);

    // query height at a known (x, y) coordinate
    float getHeight(float x, float y) const;
};

#endif
