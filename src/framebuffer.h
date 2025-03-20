#pragma once
#include <Arduino.h>
#include <TFT_eSPI.h>

#define FB_WIDTH  320
#define FB_HEIGHT 240

struct Framebuffer
{
private:
    uint16_t* m_Buffer1, *m_Buffer2;
    TFT_eSPI& m_Tft;

public:
    Framebuffer(TFT_eSPI& tft);

    void create();
    void clear(uint16_t color);
    void upload();
    void draw_pixel(uint16_t x, uint16_t y, uint16_t color);
    void draw_h_line(uint16_t x, uint16_t y, uint16_t w, uint16_t color);
    void draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
    void draw_char(uint16_t x, uint16_t y, char c, uint16_t color);
    void draw_string(uint16_t x, uint16_t y, const char* str, uint16_t color);
};

constexpr uint16_t rgb(uint8_t r, uint8_t g, uint8_t b) 
{
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}