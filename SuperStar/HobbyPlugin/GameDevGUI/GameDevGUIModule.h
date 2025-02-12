#pragma once

#include "Engine/Common/CustomMap.h"
#include "Engine/MiniEngine.h"
#include "Engine/Module/Module.h"

// モジュールのヘッダーファイルは全てインクルードする
namespace GameDevGUI
{
    /// <summary>
    /// GameDevGUIアクター用の追加モジュール
    /// </summary>
    class GameDevGUIModule final : public Module::ModuleBase
    {
        HE_MODULE_GENRATE_DECLARATION(GameDevGUIModule);

    public:
        GameDevGUIModule();

        /// <summary>
        /// GUIを新規作成
        /// </summary>
        void NewGUI(const Core::Common::Handle);

        /// <summary>
        /// GUIを破棄
        /// </summary>
        void DestoryGUI(const Core::Common::Handle);

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
#ifdef HE_USE_SDL2
        Core::Common::FixedMap<Core::Common::Handle, void*, 32> _mImGui;
#endif
    };
}  // namespace GameDevGUI
