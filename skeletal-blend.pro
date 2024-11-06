QT+=opengl
LIBS+=-lGLU
TEMPLATE = app
TARGET = ./bin/skeletal-blend
INCLUDEPATH += ./src
OBJECTS_DIR=./build/obj
MOC_DIR=./build/moc
CONFIG += c++17

# You can make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# Please consult the documentation of the deprecated API in order to know
# how to port your code away from it.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
HEADERS += src/Cartesian3.h \
           src/AnimationCycleWidget.h \
           src/BVH.h \
           src/Homogeneous4.h \
           src/HomogeneousFaceSurface.h \
           src/Matrix4.h \
           src/Scene.h \
           src/Terrain.h \
           src/Quaternion.cpp

SOURCES += src/Cartesian3.cpp \
           src/AnimationCycleWidget.cpp \
           src/BVH.cpp \
           src/Homogeneous4.cpp \
           src/HomogeneousFaceSurface.cpp \
           src/main.cpp \
           src/Matrix4.cpp \
           src/Scene.cpp \
           src/Terrain.cpp \
           src/Quaternion.cpp
