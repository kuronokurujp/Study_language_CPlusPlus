#pragma once

#include "Engine/MiniEngine.h"
#include "Engine/Platform/PlatformModule.h"

// モジュールのヘッダーファイルは全てインクルードする
namespace PlatformSDL2
{
    /// <summary>
    /// PlatformSDL2アクター用の追加モジュール
    /// </summary>
    class PlatformSDL2Module final : public Platform::PlatformModule
    {
        HE_MODULE_GENRATE_DECLARATION(PlatformSDL2Module);

    public:
        PlatformSDL2Module();

#ifdef HE_USE_SDL2
        HE::UTF8* GetOpenGLVersionNameBySDL2() override final
        {
            return HE_STR_U8_TEXT("#version 330 core");
        }
#endif

        Core::Memory::SharedPtr<Platform::TimeInterface> VTime() override final
        {
            return this->_spTime;
        }

        Core::Memory::SharedPtr<Platform::InputInterface> VInput() override final
        {
            return this->_spInput;
        }

        Core::Memory::SharedPtr<Platform::FileInterface> VFile() override final
        {
            return this->_spFile;
        }

        Core::Memory::SharedPtr<Platform::SceneInterface> VScreen() override final
        {
            return this->_spScreen;
        }

        Core::Memory::SharedPtr<Platform::SystemInterface> VSystem() override final
        {
            return this->_spSysmte;
        }

        Core::Memory::SharedPtr<Platform::FontInterface> VFont() override final
        {
            return this->_spFont;
        }

    protected:
        /// <summary>
        /// モジュール初期化
        /// </summary>
        HE::Bool _VStart() override final;

        /// <summary>
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        HE::Bool _VRelease() override final;

        /// <summary>
        /// 前更新
        /// </summary>
        void _VBeforeUpdate(const HE::Float32 in_fDeltaTime) override final;

    private:
        Core::Memory::SharedPtr<Platform::TimeInterface> _spTime;
        Core::Memory::SharedPtr<Platform::InputInterface> _spInput;
        Core::Memory::SharedPtr<Platform::FileInterface> _spFile;
        Core::Memory::SharedPtr<Platform::SceneInterface> _spScreen;
        Core::Memory::SharedPtr<Platform::SystemInterface> _spSysmte;
        Core::Memory::SharedPtr<Platform::FontInterface> _spFont;
    };
}  // namespace PlatformSDL2
