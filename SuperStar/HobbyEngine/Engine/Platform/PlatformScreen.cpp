#include "PlatformScreen.h"

namespace Platform
{
    WindowConfig::WindowConfig(const HE::Uint32 in_uW, const HE::Uint32 in_uH,
                               const HE::Uint32 in_uViewPortCount, const HE::Bool in_bMain,
                               const Core::Common::Handle in_inputHandle)

    {
        this->_bMain          = in_bMain;
        this->_uWidth         = in_uW;
        this->_uHeight        = in_uH;
        this->_uViewPortCount = in_uViewPortCount;
        this->_inputHandle    = in_inputHandle;
        this->_mMenuItem.Clear();
    }

    WindowConfig::WindowConfig(const WindowConfig& in_rOrg)
    {
        this->_bMain          = in_rOrg._bMain;
        this->_uWidth         = in_rOrg._uWidth;
        this->_uHeight        = in_rOrg._uHeight;
        this->_uViewPortCount = in_rOrg._uViewPortCount;
        this->_inputHandle    = in_rOrg._inputHandle;
        this->_mMenuItem      = in_rOrg._mMenuItem;
    }

    /// <summary>
    /// TODO: ウィンドウにメニュー項目追加
    /// </summary>
    const HE::Bool WindowConfig::AddMenuItem(const HE::Uint32 in_uID, const HE::Char* in_szName)
    {
        if (this->_mMenuItem.Contains(in_uID)) return FALSE;

        // TODO: メニュー項目を追加
        {
            auto item = WindowMenuItem();

            Core::Common::g_szTempFixedString128 = in_szName;
            Core::Common::g_szTempFixedString128.Output(item.szName, HE_ARRAY_NUM(item.szName));

            this->_mMenuItem.Add(in_uID, item);
        }

        return TRUE;
    }

    ViewPortStrategy::ViewPortStrategy(const ViewPortConfig& in_rConfig)
    {
        this->_config = in_rConfig;
    }

}  // namespace Platform
