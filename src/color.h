#pragma once
#include "macros.h"

namespace nko
{
    struct Color
    {
    public:
        union
        {
            ushort color;

            struct
            {
                ushort r : 5, g : 6, b : 5;      
            };
        };

        Color();
        Color(ushort color);
        Color(byte r, byte g, byte b);

        friend Color operator+(Color a, Color b);
        friend Color operator-(Color a, Color b);

        operator ushort() const;

        static Color blend(Color c1, Color c2, byte alpha);
    };
}