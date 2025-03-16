#include "neeko-os.h"

namespace nko
{
    OperatingSystem::OperatingSystem()
        : m_ApplicationsCount(0)
    {}

    void OperatingSystem::add_application(Application* app)
    {
        NKO_ASSERT(app, "Provided application is null!"); 
        NKO_ASSERT(m_ApplicationsCount >= max_applications, "Maximum number of applications reached!");

        m_Applications[m_ApplicationsCount++] = app;
    }
}