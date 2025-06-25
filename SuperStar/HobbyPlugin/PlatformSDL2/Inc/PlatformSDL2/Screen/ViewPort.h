#pragma once

#include "Engine/MiniEngine.h"
#include "Engine/Platform/PlatformScreen.h"

namespace PlatformSDL2
{
    /// <summary>
    /// ビューポートのロジック
    /// </summary>
    class SDL2ViewPortStrategy final : public Platform::ViewPortStrategy
    {
    public:
        SDL2ViewPortStrategy(const Platform::ViewPortConfig& in_rConfig)
            : Platform::ViewPortStrategy(in_rConfig)
        {
        }

        void VBeginRender() override final;
        void VEndRender() override final;
    };

}  // namespace PlatformSDL2
