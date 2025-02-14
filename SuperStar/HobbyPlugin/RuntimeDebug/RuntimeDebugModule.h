#pragma once
#include "Engine/MiniEngine.h"
#include "Engine/Module/Module.h"
// モジュールのヘッダーファイルは全てインクルードする
namespace RuntimeDebug
{
    /// <summary>
    /// RuntimeDebugアクター用の追加モジュール
    /// </summary>
    class RuntimeDebugModule final : public Module::ModuleBase
    {
        HE_MODULE_GENRATE_DECLARATION(RuntimeDebugModule);

    public:
        RuntimeDebugModule() : ModuleBase(ModuleName()) {}

    protected:
        /// <summary>
        /// モジュール初期化
        /// </summary>
        HE::Bool _VStart() override final;
        /// <summary>
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        HE::Bool _VRelease() override final;
    };
}  // namespace RuntimeDebug
