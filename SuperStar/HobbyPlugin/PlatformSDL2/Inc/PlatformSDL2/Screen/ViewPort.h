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

        void VBeginRender() override final;
        void VEndRender() override final;

    private:
        /// <summary>
        /// シーンの生成は継承先へ
        /// </summary>
        Core::Memory::UniquePtr<Platform::SceneStrategy> _VCreateScene(
            const Platform::SceneConfig&) override final;
    };

}  // namespace PlatformSDL2
