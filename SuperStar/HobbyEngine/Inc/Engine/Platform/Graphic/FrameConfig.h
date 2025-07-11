#pragma once

#include "Engine/Common/CustomMap.h"
#include "Engine/MiniEngine.h"

namespace Platform
{
    /// <summary>
    /// グラフィックのスクリーン設定データ
    /// </summary>
    struct FrameConfig
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
        struct MenuItem
        {
            HE::Char szName[128] = {};
        };

        using MenuItemMap = Core::Common::FixedMap<HE::Uint32, MenuItem, 32>;

    public:
        FrameConfig(const HE::Uint32 in_uW, const HE::Uint32 in_uH,
                    const HE::Uint32 in_uViewPortCount, const HE::Bool in_bMain,
                    const Core::Common::Handle in_inputHandle,
                    const EFlags in_eFlags = EFlags_None);
        FrameConfig(const FrameConfig& in_rOrg);

        inline const HE::Uint32 Width() const { return this->_uWidth; }
        inline const HE::Uint32 Height() const { return this->_uHeight; }
        inline const HE::Float32 WidthHalf() const
        {
            return static_cast<HE::Float32>(this->_uWidth) * 0.5f;
        }

        inline const HE::Float32 HeightHalf() const
        {
            return static_cast<HE::Float32>(this->_uHeight) * 0.5f;
        }

        inline const HE::Uint32 ViewPortCount() const { return this->_uViewPortCount; }
        inline const Core::Common::Handle InputHandle() const { return this->_inputHandle; }
        inline const HE::Bool IsMain() const { return this->_bMain; }
        inline const EFlags Flags() const { return this->_eFlags; }

        const MenuItemMap& GetMenuItemMap() const { return this->_mMenuItem; }

        /// <summary>
        /// メニューアイテムを追加
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
}
