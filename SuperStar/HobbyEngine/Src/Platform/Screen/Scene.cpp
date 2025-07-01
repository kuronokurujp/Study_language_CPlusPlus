#include "Engine/Platform/Screen/Scene.h"

namespace Platform
{
    SceneStrategy::SceneStrategy(const SceneConfig& in_rConfig,
                                 Core::Memory::SharedPtr<RenderInterface> in_spRenderInterface,
                                 EventRender in_eventRender)
    {
        this->_config = in_rConfig;
        this->_eventRender = std::move(in_eventRender);
        this->_spRender     = in_spRenderInterface;
    }

}  // namespace Platform
