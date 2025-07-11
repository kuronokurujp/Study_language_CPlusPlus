#pragma once

#include "Engine/Core.h"
#include "Engine/Memory/Memory.h"
#include "Engine/Module/Module.h"

// プラットフォームのインターフェイス
#include "Engine/Platform/PlatformFile.h"
#include "Engine/Platform/PlatformFont.h"
#include "Engine/Platform/PlatformInput.h"
#include "Engine/Platform/PlatformGraphic.h"
#include "Engine/Platform/PlatformSystem.h"
#include "Engine/Platform/PlatformTime.h"

namespace Platform
{
    /// <summary>
    /// プラットフォームの追加モジュール
    /// ゲームは一つのプラットフォームしか起動しない
    /// この継承したクラスのインスタンスは必ず一つになっている
    /// エンジンが起動時には必ずこのプラットフォームモジュールは設定しないとエラーになる
    /// </summary>
    class PlatformModule : public Module::ModuleBase
    {
        HE_MODULE_GENRATE_DECLARATION(PlatformModule);

    public:
        // プラットフォームモジュールはOS固有なのでモジュールレイヤーはアプリに
        PlatformModule();
        virtual ~PlatformModule() = default;

#ifdef HE_USE_SDL2
        virtual HE::UTF8* GetOpenGLVersionNameBySDL2() = 0;
#endif

        /// <summary>
        /// 時間関連の処理
        /// </summary>
        Core::Memory::SharedPtr<Platform::TimeInterface> Time() { return this->_spTime; }

        /// <summary>
        /// 入力関連
        /// </summary>
        Core::Memory::SharedPtr<Platform::InputInterface> Input() { return this->_spInput; }

        /// <summary>
        /// ファイル関連
        /// </summary>
        Core::Memory::SharedPtr<Platform::FileInterface> File() { return this->_spFile; }

        /// <summary>
        /// スクリーン関連
        /// </summary>
        Core::Memory::SharedPtr<Platform::GraphicInterface> Graphic() { return this->_spGraphic; }

        /// <summary>
        /// システム関連
        /// </summary>
        Core::Memory::SharedPtr<Platform::SystemInterface> System() { return this->_spSysmte; }

        /// <summary>
        /// フォント関連
        /// </summary>
        Core::Memory::SharedPtr<Platform::FontInterface> Font() { return this->_spFont; }

        /// <summary>
        /// プラットフォームの終了状態か
        /// </summary>
        /// <returns></returns>
        virtual HE::Bool VIsQuit() = 0;

    protected:
        /// <summary>
        /// モジュールの解放
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        virtual HE::Bool _VRelease() override;

        /// <summary>
        /// モジュール更新
        /// </summary>
        virtual void _VUpdate(const HE::Float32 in_fDeltaTime) override final;

        /// <summary>
        /// モジュール後更新
        /// </summary>
        virtual void _VLateUpdate(const HE::Float32 in_fDeltaTime) override;

    protected:
        Core::Memory::SharedPtr<Platform::TimeInterface> _spTime;
        Core::Memory::SharedPtr<Platform::InputInterface> _spInput;
        Core::Memory::SharedPtr<Platform::FileInterface> _spFile;
        Core::Memory::SharedPtr<Platform::GraphicInterface> _spGraphic;
        Core::Memory::SharedPtr<Platform::SystemInterface> _spSysmte;
        Core::Memory::SharedPtr<Platform::FontInterface> _spFont;
    };
}  // namespace Platform
