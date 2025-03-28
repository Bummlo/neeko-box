#pragma once
#include <Arduino.h>
#include <TFT_eSPI.h>
#include "application.h"
#include "bitmaps.h"
#include "macros.h"

#define DESKTOP_TIME_GMT_OFFSET         (long)3600
#define DESKTOP_TIME_DAYLIGHT_OFFSET    (int)3600
#define DESKTOP_TIME_COLOR              rgb(255, 219, 248)
#define DESKTOP_ICON_BACKGROUND         rgb(35, 35, 35)
#define DESKTOP_TETRIS_ICON_INNER_COLOR rgb(240, 159, 38)
#define DESKTOP_TETRIS_ICON_OUTER_COLOR rgb(230, 130, 44)

struct Desktop : Application
{
private:
    TFT_eSPI& m_Tft;
    char m_TimeDigits[4];

public:
    Desktop(TFT_eSPI& tft);

    virtual void init() override;
    virtual void update(uint8_t fps) override;
    virtual void slow_update(uint8_t fps) override;
    virtual void touch_input(uint16_t x, uint16_t y) override;
    virtual void button_input(Button button, ButtonAction mode) override;  
    virtual ApplicationType get_app_type() const override;

private:
    void draw_high_res_char(uint16_t x, uint16_t y, char c, uint16_t color);
    void draw_sprite_partial(uint16_t x, uint16_t y, uint16_t sprite_x, uint16_t sprite_y, uint16_t w, uint16_t h, const uint16_t* sprite);
    void draw_time_display();
    void draw_app_icons();
    void draw_settings_icon(uint16_t x, uint16_t y, uint16_t color);
    void draw_tetris_icon(uint16_t x, uint16_t y);
    void draw_top_bar(uint8_t fps);
};