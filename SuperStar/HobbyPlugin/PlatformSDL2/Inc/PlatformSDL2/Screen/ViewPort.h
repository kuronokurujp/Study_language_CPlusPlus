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
        SDL2ViewPortStrategy(const Platform::ViewPortConfig& in_rConfig);

    private:
        void _VBeginRender() override final;
        void _VEndRender() override final;

        /// <summary>
        /// シーンの生成は継承先へ
        /// </summary>
        Core::Memory::UniquePtr<Platform::SceneStrategy> _VCreateScene(
            const Platform::SceneConfig&, Core::Memory::SharedPtr<Platform::RenderInterface>,
            Platform::SceneStrategy::EventRender) override final;
    };

}  // namespace PlatformSDL2
