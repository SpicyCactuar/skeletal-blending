#include "BVH.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <queue>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

constexpr float CYLINDER_RADIUS = 0.2f;
constexpr int CYLINDER_SLICES = 10;

float easeInOut(float t);

BVH::BVH(): root(), frameCount(0), frameTime(0) {
}

// read .bvh file, basic recursive-descent parser
bool BVH::readBVHFile(const char* fileName) {
    std::ifstream inFile(fileName);
    if (inFile.bad()) {
        return false;
    }

    std::string line;
    std::vector<std::string> tokens;

    // loop through the file one line at a time
    while (std::getline(inFile, line) && line.size() != 0) {
        splitString(line, tokens);

        if (tokens[0] == "HIERARCHY") {
            // if the first token is HIERARCHY, it is the logical structure of the character
            newLine(inFile, tokens);
            Joint joint;
            readHierarchy(inFile, tokens, joint, -1);
            this->root = joint;
        } else if (tokens[0] == "MOTION") {
            // otherwise, if the first token is MOTION, it is the animation data
            readMotion(inFile);
            break;
        }
    }

    setAllJoints(this->root, this->allJoints);
    loadAllData();
    return true;
}

void BVH::newLine(std::ifstream& inFile,
                  std::vector<std::string>& tokens) {
    std::string line;
    tokens.clear();
    std::getline(inFile, line);
    splitString(line, tokens);
}

// split string with the given key character
void BVH::splitString(const std::string& input,
                      std::vector<std::string>& tokens) {
    tokens.clear();
    std::istringstream iss(input);
    while (!iss.eof()) {
        std::string token;
        iss >> token;
        tokens.push_back(token);
    }
}

// check whether the given string is a number
bool BVH::isNumeric(const std::string& str) {
    return std::all_of(str.begin(), str.end(), isdigit);
}

// recursive descent parser for the hierarchy
void BVH::readHierarchy(std::ifstream& inFile,
                        std::vector<std::string>& line,
                        Joint& joint,
                        const int parent) {
    // the new joint will have the next available ID
    joint.id = this->boneNames.size();
    joint.name = line[1];
    this->boneNames.push_back(joint.name);
    this->parentBones.push_back(parent);

    newLine(inFile, line);
    if (line[0] == "{") {
        // group of children
        // ignore the rest of the line and read in a new one
        newLine(inFile, line);
        while (line[0] != "}") {
            // until we hit the close of the group
            // The first token tells us which type of line
            if (line[0] == "OFFSET") {
                // OFFSET is the offset from the parent
                joint.offset[0] = std::stof(line[1]);
                joint.offset[1] = std::stof(line[2]);
                joint.offset[2] = std::stof(line[3]);
            } else if (line[0] == "CHANNELS") {
                // CHANNELS defines how many floats are needed for the animation, and
                // which ones
                for (int i = 0; i < std::stoi(line[1]); i++) {
                    joint.channels.push_back(line[i + 2]);
                }
            } else if (line[0] == "JOINT") {
                // JOINT defines a new joint
                Joint child;
                readHierarchy(inFile, line, child, joint.id);
                joint.children.push_back(child);
            } else if (line[0] == "End") {
                // At the leaf of the hierarchy, there is no joint. Instead it says End
                // read in and ignore three extra lines
                for (int i = 0; i < 3; i++) {
                    newLine(inFile, line);
                }
            }
            // always read the next line when done processing this line
            newLine(inFile, line);
        }
    }
}


void BVH::readMotion(std::ifstream& inFile) {
    std::string line;
    std::vector<std::string> tokens;

    // the next line should specify how many frames, so read it in
    newLine(inFile, tokens);
    this->frameCount = std::stoi(tokens[1]);

    // the next line should specify how many seconds per frame, so read it in
    newLine(inFile, tokens);
    this->frameTime = std::stof(tokens[2]);

    // after that, we loop until the end of the file
    while (std::getline(inFile, line) && line.size() != 0) {
        // split the line into tokens
        splitString(line, tokens);

        std::vector<float> frame;
        for (size_t i = 0; i < tokens.size(); i++) {
            if (!isNumeric(tokens[i])) {
                frame.push_back(std::stof(tokens[i]));
            }
        }
        this->frames.push_back(frame);
    }
}

void BVH::render(Matrix4& viewMatrix, const float scale, const int frame) {
    /**
     * According to the specification: https://research.cs.wisc.edu/graphics/Courses/cs-838-1999/Jeff/BVH.html,
     * BVH follows a right-handed system with up = Y+. We need up = Z+ for rendering.
     * Apply rotationX(90) to map Y+ -> Z+. Consequently, this makes Z+ -> Y-.
     * Apply rotationZ(180) to map Y- -> Y+, thus making (0, 1, 0) forward.
     */
    renderJoint(viewMatrix, Matrix4::rotationZ(180.0f) * Matrix4::rotationX(-90.0f), &root, scale, frame);
}

void BVH::renderJoint(Matrix4& viewMatrix,
                      const Matrix4& parentMatrix,
                      Joint* joint,
                      const float scale,
                      const int frame) {
    const Matrix4 translationMatrix = Matrix4::translation(scale * boneTranslations[joint->id]);
    // This breaks if frame < 0, which happens when (max(int) + 1) frames are rendered
    // Considered unlikely to occur for most animations
    const int frameIndex = frame % frameCount;
    const Cartesian3 rotation = boneRotations[frameIndex][joint->id];

    /**
     * Negate rotations to make bones look well oriented, uncertain of the reason
     * Could be that the BVH rotations are CW but I couldn't find proof of it
     */
    const Matrix4 rotationMatrix = Matrix4::rotationX(-rotation.x) *
                                   Matrix4::rotationY(-rotation.y) *
                                   Matrix4::rotationZ(-rotation.z);

    const Matrix4 jointMatrix = parentMatrix * translationMatrix * rotationMatrix;
    Matrix4 jointViewMatrix = viewMatrix * jointMatrix;

    for (auto& childJoint : joint->children) {
        // Bone start in Bone Coordinate System is (0, 0, 0)
        // scale * (0, 0, 0) = (0, 0, 0) => Avoid scaling
        const Cartesian3 boneStart;

        // Bone end in Bone Coordinate System is scaled child joint translation
        const Cartesian3 boneEnd = scale * boneTranslations[childJoint.id];

        renderOrientedCylinder(jointViewMatrix, boneStart, boneEnd);
        renderJoint(viewMatrix, jointMatrix, &childJoint, scale, frame);
    }
}


void BVH::renderOrientedCylinder(const Matrix4& viewMatrix,
                                 const Cartesian3& start,
                                 const Cartesian3& end) {
    // Cylinders are oriented towards (0, 0, 1)
    const Matrix4 cylinderViewMatrix =
            viewMatrix * Matrix4::rotateBetween(Cartesian3(0.0f, 0.0f, 1.0f), (end - start).unit());

    renderCylinder(cylinderViewMatrix, CYLINDER_RADIUS, (end - start).length(), CYLINDER_SLICES);
}

void BVH::renderCylinder(const Matrix4& viewMatrix,
                         const float radius,
                         const float length,
                         const int slices) {
    glBegin(GL_TRIANGLES);

    for (int i = 0; i < slices; i++) {
        // work out the angles around the main axis for the start and end of the slice
        const float theta = i * 2.0f * M_PI / slices;
        const float nextTheta = (i + 1) * 2.0f * M_PI / slices;
        const float midTheta = 0.5f * (theta + nextTheta);

        // the top vertex is always in the same place
        Homogeneous4 center_up = viewMatrix * Homogeneous4(0.0, 0.0, length, 1);
        // we have two points on the upper circle of the cylinder
        Homogeneous4 c_edge1 =
                viewMatrix * Homogeneous4(radius * std::cos(theta), radius * std::sin(theta), length, 1);
        Homogeneous4 c_edge2 =
                viewMatrix * Homogeneous4(radius * std::cos(nextTheta), radius * std::sin(nextTheta), length, 1);
        // and two points on the bottom circle
        Homogeneous4 c_edge3 =
                viewMatrix * Homogeneous4(radius * std::cos(nextTheta), radius * std::sin(nextTheta), 0, 1);
        Homogeneous4 c_edge4 = viewMatrix * Homogeneous4(radius * std::cos(theta),
                                                         radius * std::sin(theta), 0, 1);
        // and a point in the middle of the bottom
        Homogeneous4 center_bottom = viewMatrix * Homogeneous4(0.0, 0.0, 0, 1);

        // normal vectors are tricky because we need to AVOID using the translation
        // We can either use a triangle face normal, or we can do a hack ;-)
        // because we know that they are from the origin to given points

        // we have three normals: one for the top
        Cartesian3 normal_up = viewMatrix * Cartesian3(0, 0, 1.0) -
                               viewMatrix * Cartesian3(0.0, 0.0, 0.0);
        // one for the middle
        Cartesian3 normal_edge =
                viewMatrix * Cartesian3(std::cos(midTheta), std::sin(midTheta), 0.0) -
                viewMatrix * Cartesian3(0.0, 0.0, 0.0);
        // and one for the bottom
        Cartesian3 normal_bottom = viewMatrix * Cartesian3(0, 0, -1.0) -
                                   viewMatrix * Cartesian3(0.0, 0.0, 0.0);

        // render the top triangle
        glNormal3fv(&normal_up.x);
        glVertex4fv(&center_up.x);
        glVertex4fv(&c_edge1.x);
        glVertex4fv(&c_edge2.x);

        // and the side triangles
        glNormal3fv(&normal_edge.x);
        glVertex4fv(&c_edge2.x);
        glVertex4fv(&c_edge1.x);
        glVertex4fv(&c_edge4.x);

        glNormal3fv(&normal_edge.x);
        glVertex4fv(&c_edge2.x);
        glVertex4fv(&c_edge4.x);
        glVertex4fv(&c_edge3.x);

        // and the bottom triangle
        glNormal3fv(&normal_bottom.x);
        glVertex4fv(&c_edge3.x);
        glVertex4fv(&c_edge4.x);
        glVertex4fv(&center_bottom.x);
    }

    glEnd();
}

void BVH::setAllJoints(Joint& joint, std::vector<Joint*>& joint_list) {
    joint_list.push_back(&joint);

    if (joint.children.empty()) {
        return;
    }

    for (auto& i : joint.children) {
        setAllJoints(i, joint_list);
    }
}

// load all rotation and translation data into this instance
void BVH::loadAllData() {
    for (const auto& frame : this->frames) {
        std::vector<Cartesian3> frame_rotations;
        loadRotationData(frame_rotations, frame);
        this->boneRotations.push_back(frame_rotations);
    }

    for (const auto& joint : this->allJoints) {
        float x = joint->offset[0];
        float y = joint->offset[1];
        float z = joint->offset[2];
        boneTranslations.emplace_back(x, y, z);
    }
}

void BVH::loadRotationData(std::vector<Cartesian3>& rotations,
                           const std::vector<float>& frames) {
    for (size_t j = 0, j_c = 0; j < frames.size(); j_c++) {
        float rotation[3] = {0, 0, 0};
        for (size_t k = 0; k < this->allJoints[j_c]->channels.size(); k++) {
            if (this->allJoints[j_c]->channels[k].substr(1) == "rotation") {
                const int rotationIndex = bvhChannels[this->allJoints[j_c]->channels[k]];
                rotation[rotationIndex - 3] = frames[j + k];
            }
        }

        rotations.emplace_back(rotation[0], rotation[1], rotation[2]);
        j += this->allJoints[j_c]->channels.size();
    }
}

BVH* BVH::blend(const int frame, const BVH& target) const {
    BVH* blend = new BVH();

    // Hard-assumption: blend over 0.5s => 0.5s * 24 f/s = 12 frames
    blend->frameCount = 12;
    // Retain reusable properties
    blend->frameTime = this->frameTime;
    blend->root = this->root;
    blend->allJoints = this->allJoints;
    blend->parentBones = this->parentBones;
    blend->boneTranslations = this->boneTranslations;
    // Avoid initialsing blend->frames as the property unused in this codebase

    // Interpolate current frame againts first frame of target animation
    const std::vector<Cartesian3> frameRotations = this->boneRotations[frame % this->frameCount];
    const std::vector<Cartesian3> targetRotations = target.boneRotations[0];
    blend->boneRotations.resize(blend->frameCount, std::vector<Cartesian3>());
    for (unsigned int f = 0; f < blend->frameCount; f++) {
        const float t = easeInOut(f / static_cast<float>(blend->frameCount));

        for (unsigned int i = 0; i < frameRotations.size(); i++) {
            Cartesian3 interpolatedRotation = (1.0f - t) * frameRotations[i] + t * targetRotations[i];

            blend->boneRotations[f].push_back(interpolatedRotation);
        }
    }

    return blend;
}

float easeInOut(const float t) {
    const float sqt = t * t;
    return sqt / (2.0f * (sqt - t) + 1.0f);
}
