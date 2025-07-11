#include "Engine/Platform/Screen/Scene.h"

namespace Platform
{
    SceneStrategy::SceneStrategy(const SceneConfig& in_rConfig,
                                 Core::Memory::SharedPtr<DrawableInterface> in_spRenderInterface,
                                 EventDrawable in_eventRender)
    {
        this->_config = in_rConfig;
        this->_eventRender = std::move(in_eventRender);
        this->_spRender     = in_spRenderInterface;
    }

}  // namespace Platform
