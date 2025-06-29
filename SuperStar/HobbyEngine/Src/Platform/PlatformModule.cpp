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
        this->_spScreen->VRelease();

        this->_spTime.reset();
        this->_spInput.reset();
        this->_spFile.reset();
        this->_spScreen.reset();
        this->_spSysmte.reset();
        this->_spFont.reset();

        return TRUE;
    }

    void PlatformModule::_VLateUpdate(const HE::Float32 in_fDeltaTime)
    {
        // TODO: レンダリングをする
        this->_spScreen->VRendering();
    }
}  // namespace Platform
