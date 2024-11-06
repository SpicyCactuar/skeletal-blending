#ifndef ANIMATION_CYCLE_WIDGET
#define ANIMATION_CYCLE_WIDGET

#include <QtGlobal>
#include <QTimer>
#include <QMouseEvent>

// this is necessary to allow compilation in both Qt 5 and Qt 6
#if (QT_VERSION < 0x060000)
#include <QGLWidget>
#define _GEOMETRIC_WIDGET_PARENT_CLASS QGLWidget
#define _GL_WIDGET_UPDATE_CALL updateGL
#else
#include <QtOpenGLWidgets/QOpenGLWidget>
#define _GEOMETRIC_WIDGET_PARENT_CLASS QOpenGLWidget
#define _GL_WIDGET_UPDATE_CALL update
#endif

#include "Scene.h"

class AnimationCycleWidget : public _GEOMETRIC_WIDGET_PARENT_CLASS {
    Q_OBJECT

public:
    AnimationCycleWidget(QWidget* parent, Scene* scene);

protected:
    void initializeGL() override;

    void resizeGL(int width, int height) override;

    void paintGL() override;

    void keyPressEvent(QKeyEvent* event) override;

public slots:
    void nextFrame();

private:
    Scene* scene;

    QTimer* animationTimer;
};

#endif
