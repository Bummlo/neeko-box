#include "application.h"
#include "desktop.h"
#include "tetris.h"

Application* current_application;
ApplicationType desired_application;

void set_app(ApplicationType application_type)
{
    desired_application = application_type;
}

void create_desired_app(TFT_eSPI& tft)
{
    delete current_application;

    switch(desired_application)
    {
        case ApplicationType::TETRIS: current_application = new TetrisGame(tft); break;
        case ApplicationType::DESKTOP: current_application = new Desktop(tft); break;
        default: break;
    }

    current_application->init();
}

Application* get_app()
{
    return current_application;
}

ApplicationType get_desired_app()
{
    return desired_application;
}