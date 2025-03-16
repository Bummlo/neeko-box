#include "color.h"
#include <algorithm>

namespace nko
{
    Color::Color() 
        : color(0)
    {}

    Color::Color(ushort color)
        : color(color)
    {}

    Color::Color(byte r, byte g, byte b)
        : r((r * 31) / 255)
        , g((g * 63) / 255)
        , b((b * 31) / 255)
    {}
 
    Color operator+(Color a, Color b) 
    {
        return
        {
            byte(std::min(a.r + b.r, 31)),
            byte(std::min(a.g + b.g, 63)),
            byte(std::min(a.b + b.b, 31))
        };
    }

    Color operator-(Color a, Color b) 
    {
        return 
        {
            byte(std::max(a.r - b.r, 0)),
            byte(std::max(a.g - b.g, 0)),
            byte(std::max(a.b - b.b, 0))
        };
    }

    Color::operator ushort() const
    {
        return color;
    }

    Color Color::blend(Color c1, Color c2, byte alpha)
    {
        byte inv_alpha = 255 - alpha;

        byte r = ((c1.r * inv_alpha) + (c2.r * alpha)) >> 8;
        byte g = ((c1.g * inv_alpha) + (c2.g * alpha)) >> 8;
        byte b = ((c1.b * inv_alpha) + (c2.b * alpha)) >> 8;
        
        return Color(r, g, b);        
    }
}