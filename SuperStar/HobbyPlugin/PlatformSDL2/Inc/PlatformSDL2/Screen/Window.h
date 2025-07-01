#pragma once

#include "Engine/Common/CustomMap.h"
#include "Engine/MiniEngine.h"
#include "Engine/Platform/PlatformInput.h"
#include "Engine/Platform/Screen/Window.h"

namespace PlatformSDL2
{
    /// <summary>
    /// ウィンドウロジック
    /// </summary>
    class SDL2WindowStrategy final : public Platform::WindowStrategy
    {
    public:
        enum EMenuItemID
        {
            EMenuItemID_Exit = 999,
        };

    public:
        using Context = std::tuple<void*, void*>;
        using EventMenuCallback =
            std::function<void(const HE::Uint32, const Platform::WindowConfig& in_rConfig)>;

        using EventInputObjectGetter = std::function<Platform::InputObject*(Core::Common::Handle)>;

    public:
        SDL2WindowStrategy(const Platform::WindowConfig& in_rConfig,
                           EventInputObjectGetter in_inputInterfaceGetterFunc,
                           EventMenuCallback in_eventMenuCallback, Context in_pContext);

        void VUpdate(const HE::Float32 in_dt) override final;

        void VSetPos(const HE::Uint32 in_uX, const HE::Uint32 in_uY) override final;

        void VActive() override final;
        void VShow() override final;
        void VHide() override final;

#ifdef HE_USE_SDL2
        virtual void* VGetWindowBySDL2() const;
        virtual void* VGetContentBySDL2() const;
#endif

    private:
        void _VBeginRender() override final;
        void _VEndRender() override final;

        void _VBeginWindow() override final;
        void _VEndWindow() override final;

        Core::Memory::UniquePtr<Platform::ViewPortStrategy> _VCreateViewPort(
            const Platform::ViewPortConfig& in_rConfig) override final;

        /// <summary>
        /// ウィンドウにメニューアイテムを追加
        /// </summary>
        const HE::Bool _AddMenuItem(const HE::Uint32 in_uID,
                                    Platform::WindowConfig::WindowMenuItem& in_rMenuItem);

        /// <summary>
        /// メニューアイテムを押した
        /// </summary>
        void _OnMenuItem(const HE::Uint32);

    private:
        Context _context;
        HE::Uint32 _windowID = 0;
        EventInputObjectGetter _inputInterfaceGetterFunc;

        // メニューに関するイベントコールバック
        EventMenuCallback _eventMenuCallback;

        HE::Bool _bShow = FALSE;

// ウィンドウのメニュバーハンドル
#ifdef HE_WIN
        void* _hMenuBar = NULL;
#endif
    };

}  // namespace PlatformSDL2
