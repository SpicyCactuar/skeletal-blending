#ifndef SCENE
#define SCENE

#include "Terrain.h"
#include "BVH.h"
#include "Matrix4.h"
#include "Quaternion.h"

enum class AnimationState {
    Resting, Running, VeeringLeft, VeeringRight
};

class Scene {
public:
    Scene();

    void update();

    void render();

    /* Camera events */
    void eventCameraForward();

    void eventCameraLeft();

    void eventCameraRight();

    void eventCameraBackward();

    void eventCameraUp();

    void eventCameraDown();

    void eventCameraTurnLeft();

    void eventCameraTurnRight();

    /* Character events */
    void eventCharacterTurnLeft();

    void eventCharacterTurnRight();

    void eventCharacterForward();

    void eventCharacterBackward();

    void eventCharacterReset();

private:
    Terrain terrain;

    BVH restPose;
    BVH runCycle;
    BVH veerLeftCycle;
    BVH veerRightCycle;

    BVH* currentAnimation;
    // nullptr when not blending
    BVH* blendAnimation;

    AnimationState state;
    Cartesian3 characterLocation;
    Quaternion characterRotation;
    float characterSpeed;

    Matrix4 world2OpenGLMatrix;

    Matrix4 viewMatrix;
    Matrix4 cameraTranslation;
    Matrix4 cameraRotation;

    unsigned long frameNumber;

    // Defines [-x_r..x_r] and [-y_r..y_r] horizontal ranges in which the player can move
    std::pair<float, float> terrainRange;
};

#endif
