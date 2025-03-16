#include "macros.h"
#include "color.h"

namespace nko
{
    template<ushort W, ushort H>
    struct Framebuffer
    {
    private:
        Color m_Framebuffer[W * H];
    
    public:
        Framebuffer() = default;
    
        static constexpr ushort width() noexcept 
        { 
            return W; 
        }
    
        static constexpr ushort height() noexcept 
        { 
            return H; 
        }
    
        const Color* data() const
        {
            return m_Framebuffer;
        }
    
        void draw_pixel(ushort x, ushort y, Color color)
        {
            if(x < W && y < H)
            {
                m_Framebuffer[x + y * W] = color;
            }
        }
    
        void draw_circle(ushort cx, ushort cy, ushort radius, Color color)
        {
            const float edge_softness = 1.0f;  
            const float inner_radius = radius - edge_softness;  
            const float inner_radius_sq = inner_radius * inner_radius;
            const float outer_radius = radius + edge_softness;
        
            for (int y = -outer_radius; y <= outer_radius; ++y)
            {
                int py = cy + y;
    
                if (py < 0 || py >= H)
                {
                    continue;
                }
            
                for (int x = -outer_radius; x <= outer_radius; ++x)
                {
                    int px = cx + x;
                    if (px < 0 || px >= W)
                    {
                        continue;
                    }
                
                    float dist_sq = float(x * x + y * y);
                
                    if (dist_sq <= inner_radius_sq)
                    {
                        m_Framebuffer[py * W + px] = color;
                    }
                    else if (dist_sq <= outer_radius * outer_radius)
                    { 
                        float dist = sqrtf(dist_sq);                
                        float alpha = outer_radius - dist;
                   
                        alpha = (alpha > edge_softness ? edge_softness : alpha);
                        alpha = alpha / edge_softness;
                        
                        byte alpha_byte = byte(alpha * 255.0f);
                                  
                        Color bg = m_Framebuffer[py * W + px];  
                        Color blended = Color::blend(bg, color, alpha_byte);
    
                        m_Framebuffer[py * W + px] = blended;
                    }
                }
            }
        }
    
        void clear(ushort clear_color)
        {
            for(size_t i = 0; i < W * H; ++i)
            {
                m_Framebuffer[i] = clear_color;
            }
        }
    };
}