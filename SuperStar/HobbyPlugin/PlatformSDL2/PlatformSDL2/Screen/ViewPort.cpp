#include "ViewPort.h"

#include "GL/glew.h"
#include "SDL2/SDL.h"

namespace PlatformSDL2
{
    void SDL2ViewPortStrategy::VBeginRender()
    {
        ::glViewport(0, 0, this->_config._uWidth, this->_config._uHeight);
        ::glEnable(GL_MULTISAMPLE);
    }

    void SDL2ViewPortStrategy::VEndRender()
    {
        ::glDisable(GL_MULTISAMPLE);
    }
}  // namespace PlatformSDL2
