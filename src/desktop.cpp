#include "desktop.h"
#include "WiFi.h"

Desktop::Desktop(TFT_eSPI& tft)
    : m_Tft(tft) 
    , m_TimeDigits{ '0', '0', '0', '0' }
{}

void Desktop::init()
{
    if (WiFi.status() == WL_CONNECTED) 
    {
        configTime(DESKTOP_TIME_GMT_OFFSET, DESKTOP_TIME_DAYLIGHT_OFFSET, "pool.ntp.org");
    }

    m_Tft.startWrite();
    m_Tft.pushImage(0, 20, 320, 220, nino_bitmap);
    m_Tft.endWrite();

    draw_time_display();
    draw_app_icons();
    draw_top_bar(0);
}

void Desktop::update(uint8_t fps)
{
    tm current_time;

    if(getLocalTime(&current_time, 100))
    {
        char new_time_digits[4];
        sprintf(new_time_digits, "%02d%02d", current_time.tm_hour, current_time.tm_min);

        if(memcmp(m_TimeDigits, new_time_digits, 4) != 0)
        {
            memcpy(m_TimeDigits, new_time_digits, sizeof(new_time_digits));

            draw_time_display();   
        }
    }
}

void Desktop::slow_update(uint8_t fps)
{
    draw_top_bar(fps);   
}

void Desktop::touch_input(uint16_t x, uint16_t y)
{
    const uint16_t icon_size = 60;
    const uint16_t space_top = 160;
    const uint16_t screen_width = 320;

    uint16_t btn1_x = (screen_width - icon_size) / 2 - 80;
    uint16_t btn2_x = (screen_width - icon_size) / 2;
    uint16_t btn3_x = (screen_width - icon_size) / 2 + 80;
    uint16_t btn_y = space_top;

    if (x >= btn1_x && x <= btn1_x + icon_size && y >= btn_y && y <= btn_y + icon_size)
    {
        Serial.println("Button 1 Pressed");
    }
    else if (x >= btn2_x && x <= btn2_x + icon_size && y >= btn_y && y <= btn_y + icon_size)
    {
        set_app(ApplicationType::TETRIS);
    }
    else if (x >= btn3_x && x <= btn3_x + icon_size && y >= btn_y && y <= btn_y + icon_size)
    {
        Serial.println("Button 3 Pressed");
    }
}

void Desktop::button_input(Button button, ButtonAction mode)
{

}

ApplicationType Desktop::get_app_type() const
{
    return ApplicationType::DESKTOP;
}

void Desktop::draw_high_res_char(uint16_t x, uint16_t y, char c, uint16_t color) 
{
    uint8_t index;

    if(c >= '0' && c <= '9')
    {
        index = c - '0';
    } 
    else
    {
        index = 10;     // set index for ':'
    }

    const uint16_t bitmap_width = 495;
    const uint16_t bitmap_height = 65;
    const uint16_t bitmap_glyph_width = 45;
    
    size_t bytes_per_row = (bitmap_width + 7) / 8;
    uint16_t char_offset = index * bitmap_glyph_width;

    for (uint16_t gy = 0; gy < bitmap_height; ++gy) 
    {
        for (uint16_t gx = 0; gx < bitmap_glyph_width; ++gx) 
        {
            size_t actual_x = char_offset + gx;
            size_t byte_index = (gy * bytes_per_row) + (actual_x / 8);
            uint8_t bit_index = 7 - (actual_x % 8);

            if (time_font_bitmap[byte_index] & (1 << bit_index)) 
            {
                m_Tft.drawPixel(x + gx, y + gy, color);
            }
        }
    }
}

void Desktop::draw_sprite_partial(uint16_t x, uint16_t y, uint16_t sprite_x, uint16_t sprite_y, uint16_t w, uint16_t h, const uint16_t* sprite)
{
    m_Tft.setAddrWindow(x, y, w, h);

    for (uint16_t row = 0; row < h; row++) 
    {
        const uint16_t* line_buffer = &sprite[(sprite_y + row) * 320 + sprite_x];

        m_Tft.pushPixels(line_buffer, w);
    }
}

void Desktop::draw_time_display()
{
    m_Tft.startWrite();

    draw_sprite_partial(58, 30, 58, 10, 205, 65, nino_bitmap);

    draw_high_res_char(58, 30, m_TimeDigits[0], DESKTOP_TIME_COLOR);
    draw_high_res_char(103, 30, m_TimeDigits[1], DESKTOP_TIME_COLOR);
    draw_high_res_char(138, 30, ':', DESKTOP_TIME_COLOR);
    draw_high_res_char(173, 30, m_TimeDigits[2], DESKTOP_TIME_COLOR);
    draw_high_res_char(218, 30, m_TimeDigits[3], DESKTOP_TIME_COLOR);

    m_Tft.endWrite();
}

void Desktop::draw_app_icons() 
{
    const uint16_t icon_size = 60;
    const uint16_t space_top = 160;
    
    m_Tft.fillRoundRect((320 - icon_size) / 2 - 80, space_top, icon_size, icon_size, 5, DESKTOP_ICON_BACKGROUND);
    m_Tft.fillRoundRect((320 - icon_size) / 2, space_top, icon_size, icon_size, 5, DESKTOP_ICON_BACKGROUND);
    m_Tft.fillRoundRect((320 - icon_size) / 2 + 80, space_top, icon_size, icon_size, 5, DESKTOP_ICON_BACKGROUND);

    draw_tetris_icon((320 - icon_size) / 2 + 10, space_top + 10);
    //draw_settings_icon((320 - icon_size) / 2 + 90, space_top + 10, TFT_WHITE);
}

void Desktop::draw_settings_icon(uint16_t x, uint16_t y, uint16_t color) 
{
    m_Tft.drawCircle(x + 20, y + 20, 18, color);
    m_Tft.drawCircle(x + 20, y + 20, 6, color);

    for (int i = 0; i < 8; i++) 
    {
        float angle = i * PI / 4;
        int x0 = x + 20 + cos(angle) * 18;
        int y0 = y + 20 + sin(angle) * 18;
        int x1 = x + 20 + cos(angle) * 22;
        int y1 = y + 20 + sin(angle) * 22;

        m_Tft.drawLine(x0, y0, x1, y1, color);
    }
}

void Desktop::draw_tetris_icon(uint16_t x, uint16_t y)
{
    const uint16_t square_size = 13;

    m_Tft.fillRoundRect(x, y + 7, square_size * 3, square_size, 2, DESKTOP_TETRIS_ICON_OUTER_COLOR);
    m_Tft.fillRoundRect(x + square_size, y + 7, square_size , square_size * 2, 2, DESKTOP_TETRIS_ICON_OUTER_COLOR);

    m_Tft.fillRoundRect(x + 2, y + 9, square_size * 3 - 4, square_size - 4, 2, DESKTOP_TETRIS_ICON_INNER_COLOR);
    m_Tft.fillRoundRect(x + square_size + 2, y + 9, square_size - 4, square_size * 2 - 4, 2, DESKTOP_TETRIS_ICON_INNER_COLOR);
}

void Desktop::draw_top_bar(uint8_t fps)
{
    m_Tft.fillRect(0, 0, 320, 20, DESKTOP_ICON_BACKGROUND);
    m_Tft.setCursor(10, 7);
    m_Tft.print("FPS: ");
    m_Tft.print(fps);
}