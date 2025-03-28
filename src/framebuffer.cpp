#include "framebuffer.h"
#include <TFT_eSPI.h>
#include "macros.h"

Framebuffer::Framebuffer(TFT_eSPI& tft)
    : m_Buffer1(nullptr), m_Buffer2(nullptr), m_Tft(tft)
{}

Framebuffer::~Framebuffer()
{
    delete[] m_Buffer1;
    delete[] m_Buffer2; 
}

void Framebuffer::create()
{
    size_t size = FB_WIDTH * (FB_HEIGHT / 2) * sizeof(uint16_t);

    m_Buffer1 = (uint16_t*)heap_caps_malloc(size, MALLOC_CAP_DMA);
    m_Buffer2 = (uint16_t*)heap_caps_malloc(size, MALLOC_CAP_DMA);

    NKO_ASSERT(m_Buffer1, "Failed to create Framebuffer1!");
    NKO_ASSERT(m_Buffer2, "Failed to create Framebuffer2!");
}

void Framebuffer::clear(uint16_t color)
{
    for(uint16_t y = 0; y < FB_HEIGHT / 2; ++y)
    {
        for(uint16_t x = 0; x < FB_WIDTH; ++x)
        {
            m_Buffer1[x + y * FB_WIDTH] = color;
            m_Buffer2[x + y * FB_WIDTH] = color;    
        }
    }
}

void Framebuffer::upload()
{
    m_Tft.startWrite();
    m_Tft.pushImageDMA(0, 0, 320, 20, m_Buffer1);
    m_Tft.dmaWait();
    m_Tft.endWrite();
}

void Framebuffer::draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    if(x < FB_WIDTH)
    {
        if(y < FB_HEIGHT / 2)
        {
            m_Buffer1[x + y * FB_WIDTH] = color;
        }
        else if(y < FB_HEIGHT)
        {
            m_Buffer2[x + (y - FB_HEIGHT / 2) * FB_WIDTH] = color;     
        }
    }
}

void Framebuffer::draw_h_line(uint16_t x, uint16_t y, uint16_t w, uint16_t color)
{
    if (y >= FB_HEIGHT || w == 0)
    {
        return;
    }

    uint16_t end_x = x + w;

    if (end_x > FB_WIDTH)
    {
        end_x = FB_WIDTH;
    }

    uint16_t draw_w = end_x - x;
    uint16_t* buffer;

    if (y < FB_HEIGHT / 2)
    {
        buffer = m_Buffer1 + x + y * FB_WIDTH;
    }
    else
    {
        buffer = m_Buffer2 + x + (y - FB_HEIGHT / 2) * FB_WIDTH;
    }

    for (uint16_t i = 0; i < draw_w; ++i)
    {
        buffer[i] = color;
    }
}

void Framebuffer::draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    for(uint16_t ry = 0; ry < h; ++ry)
    {
        draw_h_line(x, y + ry, w, color);
    }
}

void Framebuffer::draw_char(uint16_t x, uint16_t y, char c, uint16_t color)
{
    for(uint8_t gx = 0; gx < 5; ++gx)
    {
        uint8_t glyph = pgm_read_byte(font + c * 5 + gx);

        for(uint8_t gy = 0; gy < 7; ++gy)
        {
            bool pixel = glyph >> gy & 1;

            if(pixel)
            {
                draw_pixel(x + gx, y + gy, color);
            }
        }
    }         
}

void Framebuffer::draw_string(uint16_t x, uint16_t y, const char* str, uint16_t color)
{
    while (*str)
    {
        draw_char(x, y, *str, color);
        x += 6;
        str++;
    }
}

void Framebuffer::draw_sprite(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* sprite)
{
    uint16_t sprite_end_x = x + w;
    uint16_t sprite_end_y = y + h;

    if(sprite_end_x >= FB_WIDTH)
    {
        sprite_end_x = FB_WIDTH;
    }

    if(sprite_end_y >= FB_HEIGHT)
    {
        sprite_end_y = FB_HEIGHT;
    }

    uint16_t new_w = sprite_end_x - x;
    uint16_t new_h = sprite_end_y- y;

    for(uint16_t sy = 0; sy < new_h; ++sy)
    {
        uint16_t* buffer;

        if (y + sy < FB_HEIGHT / 2)
        {
            buffer = m_Buffer1 + x + (y + sy) * FB_WIDTH;
        }
        else
        {
            buffer = m_Buffer2 + x + (y + sy - FB_HEIGHT / 2) * FB_WIDTH;
        }

        for(uint16_t sx = 0; sx < new_w; ++sx)
        {
            buffer[sx] = sprite[sx + sy * w];
        }
    }
}