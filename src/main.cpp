#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include "framebuffer.h"

TFT_eSPI tft;
XPT2046_Touchscreen ts(TOUCH_CS);
Framebuffer framebuffer(tft);

void setup() 
{
    Serial.begin(115200);

    framebuffer.create();

    ts.begin();
    ts.setRotation(3);

    tft.init();
    tft.initDMA();
    tft.setRotation(3);
    tft.setSwapBytes(true);
    tft.fillScreen(TFT_BLACK);
}

ulong lastTime = 0;
int frameCount = 0;
float fps = 0;

int shrink = 1;
int size = 10;

void loop() 
{
    frameCount++;

    for(uint16_t y = 0; y < FB_HEIGHT / 10; ++y)
    {
        for(uint16_t x = 0; x < FB_WIDTH / 10; ++x)
        {
            uint16_t color = 0;

            switch((x + y) % 5)
            {
                case 0: color = rgb(255, 24, 178); break;
                case 1: color = TFT_RED; break;
                case 2: color = TFT_BLUE; break;
                case 3: color = TFT_GREEN; break;
                case 4: color = TFT_PINK; break;
            }

            framebuffer.draw_rect(x * 10, y * 10, 10, 10, color);
        } 
    }  

    framebuffer.draw_rect(5, 5, 60, 17, TFT_BLACK);
    
    framebuffer.draw_string(10, 10, "FPS: ", TFT_WHITE);

    char buffer[10];
    itoa(fps, buffer, 10);
    
    framebuffer.draw_string(50, 10, buffer, TFT_WHITE);

    framebuffer.draw_rect(200, 150, size, size, TFT_ORANGE);
    framebuffer.draw_rect(100, 150, size, size, TFT_RED);

    size += shrink;
    
    if(size == 0 || size > 100) 
    {
        shrink *= -1;
    }

    framebuffer.upload();

    if (millis() - lastTime >= 1000) 
    {
        fps = frameCount;
        frameCount = 0;

        lastTime = millis();
    }

    if (ts.touched()) 
    {
        TS_Point p = ts.getPoint();

        float x = map(p.x, 250, 3800, tft.width(), 0);
        int y = map(p.y, 250, 3800, tft.height(), 0);

        Serial.print("Touch detected at: X: ");
        Serial.print(x);
        Serial.print(", Y: ");
        Serial.println(y);
        tft.fillCircle(x, y, 2, TFT_RED);
    }
}