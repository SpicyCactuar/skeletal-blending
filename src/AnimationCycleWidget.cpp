#include "AnimationCycleWidget.h"

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

AnimationCycleWidget::AnimationCycleWidget(QWidget* parent, Scene* scene)
    : _GEOMETRIC_WIDGET_PARENT_CLASS(parent),
      scene(scene) {
    animationTimer = new QTimer(this);
    connect(animationTimer, SIGNAL(timeout()), this, SLOT(nextFrame()));
    // set the timer to fire 24 times a second
    animationTimer->start(41.6667);
}

void AnimationCycleWidget::initializeGL() {
}

void AnimationCycleWidget::resizeGL(const int width, const int height) {
    // reset the viewport
    glViewport(0, 0, width, height);

    // set projection matrix based on zoom & window size
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // compute the aspect ratio of the widget
    const float aspectRatio = static_cast<float>(width) / height;

    // we want a 90° vertical field of view, as wide as the window allows
    // and we want to see from just in front of us to 100km away
    gluPerspective(90.0, aspectRatio, 1, 100000);

    // set model view matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void AnimationCycleWidget::paintGL() {
    scene->render();
}

void AnimationCycleWidget::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
        // exit the program
        case Qt::Key_X:
            exit(0);
        // camera controls
        case Qt::Key_W:
            scene->eventCameraForward();
            break;
        case Qt::Key_A:
            scene->eventCameraLeft();
            break;
        case Qt::Key_S:
            scene->eventCameraBackward();
            break;
        case Qt::Key_D:
            scene->eventCameraRight();
            break;
        case Qt::Key_F:
            scene->eventCameraDown();
            break;
        case Qt::Key_R:
            scene->eventCameraUp();
            break;
        case Qt::Key_Q:
            scene->eventCameraTurnLeft();
            break;
        case Qt::Key_E:
            scene->eventCameraTurnRight();
            break;
        // character controls
        case Qt::Key_P:
            scene->eventCharacterReset();
            break;
        case Qt::Key_Up:
            scene->eventCharacterForward();
            break;
        case Qt::Key_Down:
            scene->eventCharacterBackward();
            break;
        case Qt::Key_Left:
            scene->eventCharacterTurnLeft();
            break;
        case Qt::Key_Right:
            scene->eventCharacterTurnRight();
            break;
        default:
            break;
    }
}

void AnimationCycleWidget::nextFrame() {
    scene->update();

    update();
}
