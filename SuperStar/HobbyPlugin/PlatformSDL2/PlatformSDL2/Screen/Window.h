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
        SDL2WindowStrategy(const Core::Common::Handle in_handle, const Platform::WindowConfig& in_rConfig, Context in_pContext);

        void VBegin() override final;
        void VEnd() override final;
        void VUpdate(const HE::Float32) override final;

        void VShow() override final;

        void VBeginRender() override final;
        void VEndRender() override final;

#ifdef HE_USE_SDL2
        virtual void* GetWindowBySDL2() const;
        virtual void* GetContentBySDL2() const;
#endif

    private:
        Context _context;
    };

}  // namespace PlatformSDL2
