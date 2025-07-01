#pragma once

#include "Engine/MiniEngine.h"
#include "Engine/Platform/PlatformModule.h"

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

        /// <summary>
        /// プラットフォームの終了状態か
        /// </summary>
        /// <returns></returns>
        HE::Bool VIsQuit() override final;

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

        /// <summary>
        /// モジュール後更新
        /// </summary>
        void _VLateUpdate(const HE::Float32 in_fDeltaTime) override final;

    private:
        HE::Bool _bMainWindowInitialized = TRUE;
    };
}  // namespace PlatformSDL2
