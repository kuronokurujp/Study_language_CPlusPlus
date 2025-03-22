#pragma once

#include "Engine/Core.h"
#include "Engine/Memory/Memory.h"
#include "Engine/Module/Module.h"

// プラットフォームのインターフェイス
#include "PlatformFile.h"
#include "PlatformFont.h"
#include "PlatformInput.h"
#include "PlatformScreen.h"
#include "PlatformSystem.h"
#include "PlatformTime.h"

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
        PlatformModule() : ModuleBase(ModuleName(), Module::ELayer_App) {}
        virtual ~PlatformModule() = default;

#ifdef HE_USE_SDL2
        virtual HE::UTF8* GetOpenGLVersionNameBySDL2() = 0;
#endif

        /// <summary>
        /// 時間関連の処理
        /// </summary>
        virtual Core::Memory::SharedPtr<Platform::TimeInterface> VTime() = 0;

        /// <summary>
        /// 入力関連
        /// </summary>
        virtual Core::Memory::SharedPtr<Platform::InputInterface> VInput() = 0;

        /// <summary>
        /// ファイル関連
        /// </summary>
        virtual Core::Memory::SharedPtr<Platform::FileInterface> VFile() = 0;

        /// <summary>
        /// スクリーン関連
        /// </summary>
        virtual Core::Memory::SharedPtr<Platform::ScreenInterface> VScreen() = 0;

        /// <summary>
        /// システム関連
        /// </summary>
        virtual Core::Memory::SharedPtr<Platform::SystemInterface> VSystem() = 0;

        /// <summary>
        /// フォント関連
        /// </summary>
        virtual Core::Memory::SharedPtr<Platform::FontInterface> VFont() = 0;

    protected:
        /// <summary>
        /// モジュール開始
        /// </summary>
        virtual HE::Bool _VStart() override
        {
            HE_ASSERT(FALSE);
            return FALSE;
        }

        /// <summary>
        /// 前更新
        /// </summary>
        virtual void _VBeforeUpdate(const HE::Float32 in_fDeltaTime) override = 0;
    };
}  // namespace Platform
