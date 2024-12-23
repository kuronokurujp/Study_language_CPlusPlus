#include "ViewPort.h"

#include "GL/glew.h"
#include "SDL2/SDL.h"

namespace PlatformSDL2
{
    void SDL2ViewPortStrategy::VBeginRender()
    {
        // TODO: 本来ならビューポート上でやるべきだが
        ::glViewport(0, 0, this->_config._uWidth, this->_config._uHeight);
    }

    void SDL2ViewPortStrategy::VEndRender()
    {
    }
}  // namespace PlatformSDL2
