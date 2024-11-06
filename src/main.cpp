#include <iostream>
#include <string>
#include <QtWidgets/QApplication>

#include "Scene.h"
#include "AnimationCycleWidget.h"

int main(int argc, char** argv) {
    QApplication application(argc, argv);

    try {
        Scene scene;

        AnimationCycleWidget animationWindow(nullptr, &scene);
        animationWindow.resize(1200, 675);
        animationWindow.show();

        return application.exec();
    } catch (std::string errorString) {
        std::cout << "Unable to run application." << errorString << std::endl;
        return EXIT_FAILURE;
    }
}
