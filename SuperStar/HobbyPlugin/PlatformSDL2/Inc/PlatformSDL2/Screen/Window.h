#pragma once

#include "Engine/Common/CustomMap.h"
#include "Engine/MiniEngine.h"
#include "Engine/Platform/PlatformInput.h"
#include "Engine/Platform/PlatformScreen.h"

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
        void VRelease() override final;

        void VBegin() override final;
        void VEnd() override final;
        void VUpdate(const HE::Float32 in_dt) override final;

        void VSetPos(const HE::Uint32 in_uX, const HE::Uint32 in_uY) override final;

        void VActive() override final;
        void VShow() override final;
        void VHide() override final;

        void VBeginRender() override final;
        void VEndRender() override final;

#ifdef HE_USE_SDL2
        virtual void* VGetWindowBySDL2() const;
        virtual void* VGetContentBySDL2() const;
#endif

        const HE::Bool VIsClose() const override final { return this->_bClose; }

        /*
                void VRegistEventMenuCallback(EventMenuCallback in_callback) override final
                {
                    this->_eventMenuCallback = std::move(in_callback);
                }
                */

    private:
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

        HE::Bool _bClose = FALSE;
        // メニューに関するイベントコールバック
        EventMenuCallback _eventMenuCallback;

// ウィンドウのメニュバーハンドル
#ifdef HE_WIN
        void* _hMenuBar = NULL;
#endif
    };

}  // namespace PlatformSDL2
