#pragma once

#include "Engine/MiniEngine.h"
#include "Engine/Platform/PlatformScreen.h"

namespace PlatformSDL2
{
    /// <summary>
    /// 2D用のシーン
    /// </summary>
    class SDL2SceneStrategy2D : public Platform::SceneStrategyInterface
    {
    public:
        HE::Bool VBegin() override;

        void VEnd() override final;

        void VUpdate(const HE::Float32) override final;

        void VBeginRender() override final;
        void VEndRender() override final;
    };

    /// <summary>
    /// UI用のシーン
    /// </summary>
    class SDL2SceneStrategyUI final : public SDL2SceneStrategy2D
    {
    };
}  // namespace PlatformSDL2
