#include "Engine/Platform/Screen/Window.h"

namespace Platform
{
    ScreenConfig::ScreenConfig(const HE::Uint32 in_uW, const HE::Uint32 in_uH,
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

    ScreenConfig::ScreenConfig(const ScreenConfig& in_rOrg)
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
    const HE::Bool ScreenConfig::AddMenuItem(const HE::Uint32 in_uID, const HE::Char* in_szName)
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

    void Platform::Screen::_BeginViewPort()
    {
        for (auto it = this->_mViewPortSt.Begin(); it != this->_mViewPortSt.End(); ++it)
        {
            it->_data->VBegin();
        }
    }

    void Platform::Screen::_EndViewPort()
    {
        for (auto it = this->_mViewPortSt.Begin(); it != this->_mViewPortSt.End(); ++it)
        {
            it->_data->VEnd();
        }
    }

    Screen::Screen(const ScreenConfig& in_rConfig) : _config(in_rConfig)
    {
        this->_bBegin = TRUE;
    }

    void Screen::Begin()
    {
        this->_VBeginWindow();
        this->_BeginViewPort();

        this->_bBegin = FALSE;
    }

    void Screen::End()
    {
        this->_EndViewPort();
        this->_VEndWindow();
    }

    void Screen::Render()
    {
        this->_VBeginRender();
        // TODO: ビューポートのレンダリング
        for (auto it = this->_mViewPortSt.Begin(); it != this->_mViewPortSt.End(); ++it)
        {
            it->_data->Render();
        }
        this->_VEndRender();
    }

    Core::Common::Handle Screen::CreateViewPort(const Platform::ViewPortConfig& in_rConfig)
    {
        auto spSt = this->_VCreateViewPort(in_rConfig);

        Core::Common::Handle handle;
        handle.SetIndex(++this->_uViewPortStCount, 0);

        this->_mViewPortSt.Add(handle, std::move(spSt));
        return handle;
    }

    ViewPortStrategy* Screen::GetViewPort(const Core::Common::Handle in_handle)
    {
        if (this->_mViewPortSt.Contains(in_handle))
        {
            return this->_mViewPortSt.FindKey(in_handle)->_data.get();
        }

        return NULL;
    }
}  // namespace Platform
