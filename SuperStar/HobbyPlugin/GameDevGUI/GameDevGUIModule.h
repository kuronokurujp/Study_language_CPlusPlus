#pragma once

#include "./GameDevGUI/Screen/Scene.h"
#include "Engine/Common/CustomMap.h"
#include "Engine/MiniEngine.h"
#include "Engine/Module/Module.h"
#include "Engine/Platform/PlatformScreen.h"

// モジュールのヘッダーファイルは全てインクルードする
namespace GameDevGUI
{
    /// <summary>
    /// GameDevGUIアクター用の追加モジュール
    /// ImGUIは単一ウィンドウのみでしか対応できないので複数ウィンドウにUIを出す事は出来ない
    /// </summary>
    class GameDevGUIModule final : public Module::ModuleBase
    {
        HE_MODULE_GENRATE_DECLARATION(GameDevGUIModule);

    public:
        GameDevGUIModule();

        /// <summary>
        /// ImGUI用のウィンドウ戦略を作成
        /// </summary>
        Core::Memory::UniquePtr<Platform::WindowStrategy> CreateWindowStrategy(
            const Core::Common::Handle, const Platform::WindowConfig&);

        /// <summary>
        /// ImGUI用のシーン戦略を作成
        /// </summary>
        Core::Memory::UniquePtr<GameDevGUISceneStrategy> CreateSceneStrategy();

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
        /// モジュール更新
        /// </summary>
        void _VUpdate(const HE::Float32 in_fDeltaTime) override final;

    private:
        Core::Common::Handle _windowHandle;
    };
}  // namespace GameDevGUI
