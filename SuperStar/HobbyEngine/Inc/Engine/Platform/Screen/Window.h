#pragma once

#include "Engine/Common/CustomMap.h"
#include "Engine/MiniEngine.h"
#include "Engine/Platform/Screen/ViewPort.h"

namespace Platform
{
    // 前方宣言
    class ViewPortStrategy;

    /// <summary>
    /// ウィンドウの設定データ
    /// </summary>
    struct WindowConfig
    {
    public:
        enum EFlags
        {
            EFlags_None               = 0x00000000,
            EFlags_Resizable          = 0x00000010,
            EFlags_Minimized          = 0x00000020,
            EFlags_Maximized          = 0x00000040,
            EFlags_WinDisableCloseBtn = 0x00000080,
        };

        /// <summary>
        /// ウィンドウのメニュー項目について
        /// メニュー項目の入れ子項目が今は作れない
        /// </summary>
        struct WindowMenuItem
        {
            HE::Char szName[128] = {};
        };

        using MenuItemMap = Core::Common::FixedMap<HE::Uint32, WindowMenuItem, 32>;

    public:
        WindowConfig(const HE::Uint32 in_uW, const HE::Uint32 in_uH,
                     const HE::Uint32 in_uViewPortCount, const HE::Bool in_bMain,
                     const Core::Common::Handle in_inputHandle,
                     const EFlags in_eFlags = EFlags_None);
        WindowConfig(const WindowConfig& in_rOrg);

        inline const HE::Uint32 Width() const { return this->_uWidth; }
        inline const HE::Uint32 Height() const { return this->_uHeight; }
        inline const HE::Uint32 ViewPortCount() const { return this->_uViewPortCount; }
        inline const Core::Common::Handle InputHandle() const { return this->_inputHandle; }
        inline const HE::Bool IsMain() const { return this->_bMain; }
        inline const EFlags Flags() const { return this->_eFlags; }

        const MenuItemMap& GetMenuItemMap() const { return this->_mMenuItem; }

        /// <summary>
        /// TODO: ウィンドウにメニューアイテムを追加
        /// </summary>
        const HE::Bool AddMenuItem(const HE::Uint32 in_uID, const HE::Char* in_szName);

    private:
        EFlags _eFlags             = EFlags_None;
        HE::Uint32 _uWidth         = 0;
        HE::Uint32 _uHeight        = 0;
        HE::Uint32 _uViewPortCount = 0;
        HE::Bool _bMain            = FALSE;
        Core::Common::Handle _inputHandle;

        MenuItemMap _mMenuItem;
    };

    /// <summary>
    /// ウィンドウのロジック
    /// </summary>
    class WindowStrategy
    {
    public:
        using MapViewPortSt =
            Core::Common::FixedMap<Core::Common::Handle,
                                   Core::Memory::UniquePtr<Platform::ViewPortStrategy>, 32>;

        /// <summary>
        /// メニューイベントのコールバック型
        /// </summary>
        using EventMenuCallback = std::function<void(HE::Uint32)>;

    public:
        WindowStrategy(const WindowConfig& in_rConfig);  //

        virtual void VSetPos(const HE::Uint32 in_uX, const HE::Uint32 in_uY) = 0;

        virtual void VActive() = 0;
        virtual void VShow()   = 0;
        virtual void VHide()   = 0;

        void Begin();
        void End();
        virtual void VUpdate(const HE::Float32 in_dt) = 0;

        void Render();

        /// <summary>
        /// TODO: 引数にストラテジーを設定できるようにする
        /// </summary>
        Core::Common::Handle CreateViewPort(const Platform::ViewPortConfig& in_rConfig);
        ViewPortStrategy* GetViewPort(const Core::Common::Handle);

#ifdef HE_USE_SDL2
        virtual void* VGetWindowBySDL2() const  = 0;
        virtual void* VGetContentBySDL2() const = 0;
#endif
        inline HE::Bool IsBegin() const { return this->_bBegin; }

        inline const WindowConfig& GetConfig() const { return this->_config; }

    protected:
        virtual void _VBeginWindow() = 0;
        virtual void _VEndWindow()   = 0;

        virtual void _VBeginRender() = 0;
        virtual void _VEndRender()   = 0;

        /// <summary>
        /// TODO: ViewPortの生成は継承先へ
        /// </summary>
        virtual Core::Memory::UniquePtr<Platform::ViewPortStrategy> _VCreateViewPort(
            const Platform::ViewPortConfig& in_rConfig) = 0;

    private:
        void _BeginViewPort();
        void _EndViewPort();

    private:
        HE::Bool _bBegin = FALSE;
        Platform::WindowConfig _config;

        MapViewPortSt _mViewPortSt;
        HE::Uint32 _uViewPortStCount = 0;
    };
}  // namespace Platform
