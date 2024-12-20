#pragma once

#include "Engine/MiniEngine.h"
#include "Engine/Platform/PlatformScreen.h"

namespace PlatformSDL2
{
    /// <summary>
    /// ウィンドウロジック
    /// </summary>
    class SDL2WindowStrategy final : public Platform::WindowStrategy
    {
    public:
        SDL2WindowStrategy(const Platform::WindowConfig& in_rConfig, void* in_pGLContext);

        void VBegin() override final;
        void VEnd() override final;

        void VShow() override final;

        void VBeginRender() override final;
        void VEndRender() override final;

    private:
        void* _pWindow  = NULL;
        void* _pContext = NULL;
    };

}  // namespace PlatformSDL2
