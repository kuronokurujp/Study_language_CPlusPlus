#include "Engine/Platform/PlatformModule.h"

namespace Platform
{
    // プラットフォームモジュールはOS固有なのでモジュールレイヤーはアプリに
    PlatformModule::PlatformModule() : ModuleBase(ModuleName(), Module::ELayer_App)
    {
    }

    HE::Bool PlatformModule::_VRelease()
    {
        // 各機能の解放
        this->_spInput->VRelease();
        this->_spFont->VRelease();
        this->_spGraphic->VRelease();

        this->_spTime.reset();
        this->_spInput.reset();
        this->_spFile.reset();
        this->_spGraphic.reset();
        this->_spSysmte.reset();
        this->_spFont.reset();

        return TRUE;
    }

    void PlatformModule::_VUpdate(const HE::Float32 in_fDeltaTime)
    {
        this->_spGraphic->VUpdate(in_fDeltaTime);
    }

    void PlatformModule::_VLateUpdate(const HE::Float32 in_fDeltaTime)
    {
        // グラフィックの描画
        this->_spGraphic->VDraw();
    }
}  // namespace Platform
