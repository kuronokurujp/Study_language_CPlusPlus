#pragma once

#include "Engine/Common/CustomMap.h"
#include "Engine/MiniEngine.h"
#include "Engine/Platform/Graphic/FrameBase.h"
#include "Engine/Platform/PlatformInput.h"

namespace PlatformSDL2
{
    /// <summary>
    /// SDL2のグラフィックフレーム
    /// </summary>
    class Frame final : public Platform::FrameBase
    {
    public:
        enum EMenuItemID
        {
            EMenuItemID_Exit = 999,
        };

    public:
        using Context = std::tuple<void*, void*>;
        using EventMenuCallback =
            std::function<void(const HE::Uint32, const Platform::FrameConfig&)>;

        using EventInputObjectGetter = std::function<Platform::InputObject*(Core::Common::Handle)>;

    public:
        Frame(const Platform::FrameConfig&, Core::Memory::UniquePtr<Platform::EventFrameInterface>,
              EventInputObjectGetter, EventMenuCallback, Context);

        void VSetPos(const HE::Uint32 in_uX, const HE::Uint32 in_uY) override final;

        void VActive() override final;
        void VShow() override final;
        void VHide() override final;

        void VBegin() override final;
        void VEnd() override final;
        void VUpdate(const HE::Float32 in_dt) override final;

        void VDraw(Platform::MapDrawable&) override final;

#ifdef HE_USE_SDL2
        virtual void* VGetWindowBySDL2() const;
        virtual void* VGetContentBySDL2() const;
#endif

    private:
        /// <summary>
        /// ウィンドウにメニューアイテムを追加
        /// </summary>
        const HE::Bool _AddMenuItem(const HE::Uint32 in_uID, Platform::FrameConfig::MenuItem&);

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
