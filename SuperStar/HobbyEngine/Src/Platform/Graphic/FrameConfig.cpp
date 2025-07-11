#include "Engine/Platform/Graphic/FrameConfig.h"

namespace Platform
{
    FrameConfig::FrameConfig(const HE::Uint32 in_uW, const HE::Uint32 in_uH,
                             const HE::Uint32 in_uViewPortCount, const HE::Bool in_bMain,
                             const Core::Common::Handle in_inputHandle, const EFlags in_eFlags)

    {
        this->_bMain          = in_bMain;
        this->_uWidth         = in_uW;
        this->_uHeight        = in_uH;
        this->_uViewPortCount = in_uViewPortCount;
        this->_inputHandle    = in_inputHandle;
        this->_eFlags         = in_eFlags;
        this->_mMenuItem.Clear();
    }

    FrameConfig::FrameConfig(const FrameConfig& in_rOrg)
    {
        this->_bMain          = in_rOrg._bMain;
        this->_uWidth         = in_rOrg._uWidth;
        this->_uHeight        = in_rOrg._uHeight;
        this->_uViewPortCount = in_rOrg._uViewPortCount;
        this->_inputHandle    = in_rOrg._inputHandle;
        this->_mMenuItem      = in_rOrg._mMenuItem;
        this->_eFlags         = in_rOrg._eFlags;
    }

    /// <summary>
    /// ウィンドウにメニュー項目追加
    /// </summary>
    const HE::Bool FrameConfig::AddMenuItem(const HE::Uint32 in_uID, const HE::Char* in_szName)
    {
        if (this->_mMenuItem.Contains(in_uID)) return FALSE;

        // メニュー項目を追加
        {
            auto item = MenuItem();

            Core::Common::g_szTempFixedString128 = in_szName;
            Core::Common::g_szTempFixedString128.Output(item.szName, HE_ARRAY_NUM(item.szName));

            this->_mMenuItem.Add(in_uID, item);
        }

        return TRUE;
    }
}  // namespace Platform
