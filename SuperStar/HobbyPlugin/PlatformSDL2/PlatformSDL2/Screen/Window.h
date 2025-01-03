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
        using Context = std::tuple<void*, void*>;

    public:
        SDL2WindowStrategy(const Platform::WindowConfig& in_rConfig, Context in_pContext);

        void VBegin() override final;
        void VEnd() override final;

        void VShow() override final;

        void VBeginRender() override final;
        void VEndRender() override final;

    private:
        // void* _pWindow = NULL;
        Context _context;
    };

}  // namespace PlatformSDL2
