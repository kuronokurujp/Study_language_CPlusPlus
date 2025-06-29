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

    void SDL2ViewPortStrategy::VBeginRender()
    {
        ::glViewport(0, 0, this->_config._uWidth, this->_config._uHeight);
        ::glEnable(GL_DEPTH_TEST);
        ::glDisable(GL_BLEND);

        //::glEnable(GL_MULTISAMPLE);

        for (auto it = this->_mSceneSt.Begin(); it != this->_mSceneSt.End(); ++it)
        {
            it->_data->VBeginRender();
        }
    }

    void SDL2ViewPortStrategy::VEndRender()
    {
        for (auto it = this->_mSceneSt.Begin(); it != this->_mSceneSt.End(); ++it)
        {
            it->_data->VEndRender();
        }

        //::glDisable(GL_MULTISAMPLE);
    }

    Core::Memory::UniquePtr<Platform::SceneStrategy> SDL2ViewPortStrategy::_VCreateScene(
        const Platform::SceneConfig& in_rConfig)
    {
        // TODO: 2D/3Dでシーンを切り替えるかも
        auto upScene = HE_MAKE_CUSTOM_UNIQUE_PTR((SDL2SceneStrategy2D), in_rConfig);
        return std::move(upScene);
    }

}  // namespace PlatformSDL2
