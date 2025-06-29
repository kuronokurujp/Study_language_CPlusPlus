#pragma once

#include "Engine/MiniEngine.h"
#include "Engine/Platform/PlatformScreen.h"

namespace PlatformSDL2
{
    /// <summary>
    /// 2D用のシーン
    /// </summary>
    class SDL2SceneStrategy2D : public Platform::SceneStrategy
    {
    public:
        SDL2SceneStrategy2D(const Platform::SceneConfig&);

        HE::Bool VBegin() override;
        void VUpdate(const HE::Float32) override final;
        void VEnd() override final;

        void VBeginRender() override final;
        void VEndRender() override final;
    };
}  // namespace PlatformSDL2
