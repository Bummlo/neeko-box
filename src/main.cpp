#include <Arduino.h>
#include <TFT_eSPI.h>
#include "neeko-os.h"
#include "framebuffer.h"

// TEST S3de

TFT_eSPI tft = TFT_eSPI();

struct Calculator : nko::Application
{
    virtual void app_start() override
    {

    }
};

nko::OperatingSystem os;

void setup() 
{
    nko::Framebuffer<320, 240> f;

    Serial.begin(115200);

    while(!Serial) {}

    Serial.println("Initalized serial connection to ESP32!");

    Calculator* calc = new Calculator;

    os.add_application(calc);
}

void loop() 
{
    Serial.println("Hi :3");

    delay(500);
}