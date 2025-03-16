#pragma once
#include <vector>
#include <Arduino.h>
#include "macros.h"

namespace nko
{
    struct Application
    {
    public:
        const char* app_name;
        const char* app_version;
    
        virtual void app_start() = 0;
    };
    
    struct OperatingSystem
    {
    private:
        static constexpr const char* os_name = "NeekoOS";
        static constexpr const char* os_version = "1.0.0";
        static constexpr byte max_applications = 16;
        static constexpr int display_width = 320;
        static constexpr int display_height = 240;
    
        Application* m_Applications[max_applications];
        size_t m_ApplicationsCount;
    
    public:
        OperatingSystem();

        void add_application(Application* app);
    };
}