#include "PlatformSDL2/Screen/ViewPort.h"

#include "GL/glew.h"
#include "PlatformSDL2/Screen/Scene.h"
#include "SDL2/SDL.h"

namespace PlatformSDL2
{
    SDL2ViewPortStrategy::SDL2ViewPortStrategy(const Platform::ViewPortConfig& in_rConfig)
        : Platform::ViewPortStrategy(in_rConfig)
    {
    }

    void SDL2ViewPortStrategy::_VBeginRender()
    {
        ::glViewport(0, 0, this->_config._uWidth, this->_config._uHeight);
        ::glEnable(GL_DEPTH_TEST);
        ::glDisable(GL_BLEND);

        //::glEnable(GL_MULTISAMPLE);
    }

    void SDL2ViewPortStrategy::_VEndRender()
    {
        //::glDisable(GL_MULTISAMPLE);
    }

    Core::Memory::SharedPtr<Platform::SceneStrategy> SDL2ViewPortStrategy::_VCreateScene(
        const Platform::SceneConfig& in_rConfig,
        Core::Memory::SharedPtr<Platform::RenderInterface> in_spRenderInterface,
        Platform::SceneStrategy::EventRender in_eventRender)
    {
        // TODO: 2D/3Dでシーンを切り替えるかも
        auto upScene = HE_MAKE_CUSTOM_SHARED_PTR((SDL2SceneStrategy2D), in_rConfig,
                                                 in_spRenderInterface, std::move(in_eventRender));
        return upScene;
    }

}  // namespace PlatformSDL2
