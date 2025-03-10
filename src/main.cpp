#include "mfw.h"

class Simulation : public mfw::Application {
public:
    Simulation(): Application("Ball Simulation", 1280, 720)
    {}

};

mfw::Application* mfw::CreateApplication() {
    return new Simulation();
}
