#ifndef BVH_H
#define BVH_H

#include <array>
#include <vector>
#include <string>
#include <map>

#include "Cartesian3.h"
#include "Matrix4.h"

class Joint {
public:
    // index of Joint
    int id;
    std::string name;
    std::array<float, 3> offset{0.0f, 0.0f, 0.0f};
    std::vector<std::string> channels;
    std::vector<Joint> children;
};

// Biovision hierarchical data
// https://research.cs.wisc.edu/graphics/Courses/cs-838-1999/Jeff/BVH.html
class BVH {
public:
    Joint root;

    int frameCount;

private:
    std::map<std::string, int> bvhChannels{
        {"Xposition", 0},
        {"Yposition", 1},
        {"Zposition", 2},
        {"Xrotation", 3},
        {"Yrotation", 4},
        {"Zrotation", 5}
    };

public:
    // constructor
    BVH();

    // render bvh animation by given a sequence of frames data
    void render(Matrix4& viewMatrix, float scale, int frame);

    // Routines for file I/O
    // read data from bvh file
    bool readBVHFile(const char* fileName);

    // creates a BVH that represents the blend of this into target
    BVH* blend(int frame, const BVH& target) const;

private:
    float frameTime;

    std::vector<std::string> boneNames;
    std::vector<Joint*> allJoints;
    // id -> parent id
    std::vector<int> parentBones;

    // a vector to store all frames of the animation
    // this is *JUST* a huge 2D array of floats
    // in each frame, we have six channels for position and rotation for each joint
    // listed in strict numerical order
    std::vector<std::vector<float>> frames;

    std::vector<Cartesian3> boneTranslations;
    std::vector<std::vector<Cartesian3>> boneRotations;

    static void newLine(std::ifstream&, std::vector<std::string>&);

    static void splitString(const std::string&, std::vector<std::string>&);

    void readHierarchy(std::ifstream&, std::vector<std::string>&, Joint&, int parent);

    void readMotion(std::ifstream&);

    // load all rotation and translation data into this class
    void loadAllData();

    void loadRotationData(std::vector<Cartesian3>& rotations, const std::vector<float>& frames);

    static bool isNumeric(const std::string&);

    // render a single joint by given frame index
    void renderJoint(Matrix4& viewMatrix, const Matrix4& parentMatrix, Joint* joint, float scale, int frame);

    // render cylinder given the start position and the end position
    static void renderOrientedCylinder(const Matrix4& viewMatrix, const Cartesian3& start, const Cartesian3& end);

    static void renderCylinder(const Matrix4& viewMatrix, float radius, float length, int slices);

    static void setAllJoints(Joint&, std::vector<Joint*>&);
};

#endif
