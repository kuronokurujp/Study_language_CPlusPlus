#pragma once

#include "Engine/MiniEngine.h"
#include "Engine/Platform/PlatformGraphic.h"

namespace PlatformSDL2
{
    /// <summary>
    /// 2D用のシーン
    /// </summary>
    class SDL2SceneStrategy2D : public Platform::SceneStrategy
    {
    public:
        SDL2SceneStrategy2D(const Platform::SceneConfig&,
                            Core::Memory::SharedPtr<Platform::RenderInterface>, EventRender);

        HE::Bool VBegin() override;
        void VEnd() override final;

        void VRender() override final;
    };
}  // namespace PlatformSDL2
