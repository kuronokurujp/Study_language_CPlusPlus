#pragma once

#include "Engine/Common/CustomMap.h"
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
        SDL2WindowStrategy(const Core::Common::Handle in_handle,
                           const Platform::WindowConfig& in_rConfig, Context in_pContext);
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

        void VRegistEventMenuCallback(EventMenuCallback in_callback) override final
        {
            this->_eventMenuCallback = std::move(in_callback);
        }

        const Core::Common::Handle VGetHandle() const override final { return this->_handle; }
        const Platform::WindowConfig& VGetConfig() const override final { return *this->_pConfig; }

    private:
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

        Core::Common::Handle _handle;
        Platform::WindowConfig* _pConfig = NULL;
        HE::Bool _bClose                 = FALSE;
        // メニューに関するイベントコールバック
        EventMenuCallback _eventMenuCallback;

        // ウィンドウのメニュバーハンドル
#ifdef HE_WIN
        void* _hMenuBar = NULL;
#endif
    };

}  // namespace PlatformSDL2
