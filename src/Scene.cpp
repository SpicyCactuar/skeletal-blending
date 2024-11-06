#include "Scene.h"

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

#include <limits>
#include <cmath>
#include <algorithm>
#include <array>

// three local variables with the hardcoded file names
const std::string terrainName = "assets/randomland.dem";
const std::string motionBvhStand = "assets/stand.bvh";
const std::string motionBvhRun = "assets/fast_run.bvh";
const std::string motionBvhVeerLeft = "assets/veer_left.bvh";
const std::string motionBvhVeerRight = "assets/veer_right.bvh";
constexpr float cameraSpeed = 0.5;

const Homogeneous4 sunDirection(0.5, -0.5, 0.3, 1.0);
constexpr std::array<float, 4> groundColour = {0.2, 0.5, 0.2, 1.0};
constexpr std::array<float, 4> boneColour = {0.6, 0.0, 0.54, 1.0};
constexpr std::array<float, 4> sunAmbient = {0.1, 0.1, 0.1, 1.0};
constexpr std::array<float, 4> sunDiffuse = {0.7, 0.7, 0.7, 1.0};
constexpr std::array<float, 4> blackColour = {0.0, 0.0, 0.0, 1.0};

// Measured in units
constexpr float terrainPadding = 16.0f;

// Measured in units/frame
constexpr float speedDelta = 1.0f;

// Direction
const Cartesian3 forward(0.0f, 1.0f, 0.0f);
const Cartesian3 up(0.0f, 0.0f, 1.0f);

// Scales the animation model
constexpr float bvhScale = 0.1f;

// Veering
// Account for Quaternion factor
constexpr float veerRotationTheta = 45.0f / 2.0f;
constexpr unsigned int veerFrames = 33;
Quaternion veerFrom;
Quaternion veerTo;

// constructor
Scene::Scene() {
    // load the terrain
    terrain.readTerrainFile(terrainName.data(), 3);
    const float terrainRangeX = terrain.heightValues.size() * terrain.xyScale;
    const float terrainRangeY = terrain.heightValues[0].size() * terrain.xyScale / 4;
    terrainRange = std::make_pair(terrainRangeX - terrainPadding, terrainRangeY - terrainPadding);

    // load the animation data
    restPose.readBVHFile(motionBvhStand.data());
    runCycle.readBVHFile(motionBvhRun.data());
    veerLeftCycle.readBVHFile(motionBvhVeerLeft.data());
    veerRightCycle.readBVHFile(motionBvhVeerRight.data());

    // set initial camera
    world2OpenGLMatrix = Matrix4::rotationX(90.0);
    cameraTranslation = Matrix4::translation(Cartesian3(-5, 15, -15.5));
    cameraRotation = Matrix4::rotationX(-30.0) * Matrix4::rotationZ(15.0);

    // initialize the character's position and rotation
    eventCharacterReset();
}

void Scene::update() {
    // increment the frame counter
    frameNumber++;

    // After blending finishes, discard data
    // Restart frameNumber to smoothly transition between blendingAnimation -> currentAnimation
    if (blendAnimation != nullptr && frameNumber >= blendAnimation->frameCount) {
        frameNumber = 0;
        delete blendAnimation;
        blendAnimation = nullptr;
    }

    if (state == AnimationState::VeeringLeft || state == AnimationState::VeeringRight) {
        if (frameNumber < veerFrames) {
            // Character is veering, slerp rotation
            const float t = frameNumber / static_cast<float>(veerFrames);
            characterRotation = slerp(veerFrom, veerTo, t);
        } else {
            // Blend into run or rest, depending on the preserved speed
            state = characterSpeed > 0.0f ? AnimationState::Running : AnimationState::Resting;
            BVH& nextAnimation = state == AnimationState::Running ? runCycle : restPose;
            blendAnimation = currentAnimation->blend(frameNumber, nextAnimation);
            frameNumber = 0;
            currentAnimation = &nextAnimation;
        }
    }

    // move character along the terrain plane, respecting bounds
    const Cartesian3 translation = characterSpeed * (characterRotation.matrix() * forward);
    Cartesian3 updatedXY = characterLocation + translation;
    updatedXY.x = std::clamp(updatedXY.x, -terrainRange.first, terrainRange.first);
    updatedXY.y = std::clamp(updatedXY.y, -terrainRange.second, terrainRange.second);

    // place character on top of the terrain
    const float updatedZ = terrain.getHeight(updatedXY.x, updatedXY.y);

    // update character location with new coordinates
    characterLocation = Cartesian3(updatedXY.x, updatedXY.y, updatedZ);
}

void Scene::render() {
    // enable Z-buffering
    glEnable(GL_DEPTH_TEST);

    // set lighting parameters
    glShadeModel(GL_FLAT);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0, GL_AMBIENT, sunAmbient.data());
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sunDiffuse.data());
    glLightfv(GL_LIGHT0, GL_SPECULAR, blackColour.data());
    glLightfv(GL_LIGHT0, GL_EMISSION, blackColour.data());

    // background is sky-blue
    glClearColor(0.7, 0.7, 1.0, 1.0);

    // clear the buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // compute the view matrix by combining camera translation, rotation & world2OpenGL
    viewMatrix = world2OpenGLMatrix * cameraRotation * cameraTranslation;

    // compute the light position
    Homogeneous4 lightDirection = world2OpenGLMatrix * cameraRotation * sunDirection;

    // turn it into Cartesian and normalise
    const Cartesian3 lightVector = lightDirection.Vector().unit();

    // and set the w to zero to force infinite distance
    lightDirection.w = 0.0;

    // pass it to OpenGL
    glLightfv(GL_LIGHT0, GL_POSITION, &lightVector.x);

    // and set a material colour for the ground
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, groundColour.data());
    glMaterialfv(GL_FRONT, GL_SPECULAR, blackColour.data());
    glMaterialfv(GL_FRONT, GL_EMISSION, blackColour.data());

    // render the terrain
    terrain.render(viewMatrix);

    // now set the colour to draw the bones
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, boneColour.data());

    // render the character
    Matrix4 frameViewMatrix = viewMatrix * Matrix4::translation(characterLocation) * characterRotation.matrix();
    // Not the solution we deserve, but the one we need
    if (blendAnimation != nullptr) {
        blendAnimation->render(frameViewMatrix, bvhScale, frameNumber);
    } else {
        currentAnimation->render(frameViewMatrix, bvhScale, frameNumber);
    }
}

void Scene::eventCameraForward() {
    cameraTranslation = cameraTranslation *
                        cameraRotation.transpose() *
                        Matrix4::translation(Cartesian3(0.0f, -cameraSpeed, 0.0f)) *
                        cameraRotation;
}

void Scene::eventCameraBackward() {
    cameraTranslation = cameraTranslation *
                        cameraRotation.transpose() *
                        Matrix4::translation(Cartesian3(0.0f, cameraSpeed, 0.0f)) *
                        cameraRotation;
}

void Scene::eventCameraLeft() {
    cameraTranslation = cameraTranslation *
                        cameraRotation.transpose() *
                        Matrix4::translation(Cartesian3(cameraSpeed, 0.0f, 0.0f)) *
                        cameraRotation;
}

void Scene::eventCameraRight() {
    cameraTranslation = cameraTranslation *
                        cameraRotation.transpose() *
                        Matrix4::translation(Cartesian3(-cameraSpeed, 0.0f, 0.0f)) *
                        cameraRotation;
}

// camera control events: RF for vertical motion
void Scene::eventCameraUp() {
    cameraTranslation = cameraTranslation *
                        cameraRotation.transpose() *
                        Matrix4::translation(Cartesian3(0.0f, 0.0f, -cameraSpeed)) *
                        cameraRotation;
}

void Scene::eventCameraDown() {
    cameraTranslation = cameraTranslation *
                        cameraRotation.transpose() *
                        Matrix4::translation(Cartesian3(0.0f, 0.0f, cameraSpeed)) *
                        cameraRotation;
}

void Scene::eventCameraTurnLeft() {
    cameraRotation = cameraRotation * Matrix4::rotationZ(2.0f);
}

void Scene::eventCameraTurnRight() {
    cameraRotation = cameraRotation * Matrix4::rotationZ(-2.0f);
}

void Scene::eventCharacterTurnLeft() {
    if (state == AnimationState::VeeringLeft) return;

    state = AnimationState::VeeringLeft;
    blendAnimation = currentAnimation->blend(frameNumber, veerLeftCycle);
    frameNumber = 0;
    veerFrom = characterRotation;
    veerTo = characterRotation * Quaternion(up, veerRotationTheta);
    currentAnimation = &veerLeftCycle;
}

void Scene::eventCharacterTurnRight() {
    if (state == AnimationState::VeeringRight) return;

    state = AnimationState::VeeringRight;
    blendAnimation = currentAnimation->blend(frameNumber, veerRightCycle);
    frameNumber = 0;
    veerFrom = characterRotation;
    veerTo = characterRotation * Quaternion(up, -veerRotationTheta);
    currentAnimation = &veerRightCycle;
}

void Scene::eventCharacterForward() {
    if (state == AnimationState::Running) return;

    state = AnimationState::Running;
    blendAnimation = currentAnimation->blend(frameNumber, runCycle);
    frameNumber = 0;
    characterSpeed = speedDelta;
    currentAnimation = &runCycle;
}

void Scene::eventCharacterBackward() {
    if (state == AnimationState::Resting) return;

    state = AnimationState::Resting;
    blendAnimation = currentAnimation->blend(frameNumber, restPose);
    frameNumber = 0;
    characterSpeed = 0.0f;
    currentAnimation = &restPose;
}

void Scene::eventCharacterReset() {
    this->characterLocation = Cartesian3(0, 0, 0);
    this->characterRotation = Quaternion(up, 0.0f);
    this->characterSpeed = 0.0f;
    this->state = AnimationState::Resting;
    this->currentAnimation = &restPose;
    this->frameNumber = 0;
}
