#pragma once
#include <Arduino.h>
#include <TFT_eSPI.h>

enum struct ApplicationType
{
    DESKTOP,
    TETRIS
};

enum struct ButtonAction
{
    PRESSED,
    BUTTON_DOWN,
    RELEASED
};

enum struct Button
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    A,
    B
};

struct Application
{
public:
    virtual void init() = 0;
    virtual void update(uint8_t fps) = 0;
    virtual void slow_update(uint8_t fps) = 0;
    virtual void touch_input(uint16_t x, uint16_t y) = 0;
    virtual void button_input(Button button, ButtonAction mode) = 0;

    virtual ApplicationType get_app_type() const = 0;
};

void set_app(ApplicationType application_type);

void create_desired_app(TFT_eSPI& tft);

Application* get_app();

ApplicationType get_desired_app();