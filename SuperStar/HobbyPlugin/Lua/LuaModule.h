#pragma once

#include "Engine/MiniEngine.h"
#include "Engine/Module/Module.h"

// モジュールのヘッダーファイルは全てインクルードする
namespace Lua
{
    /// <summary>
    /// Luaアクター用の追加モジュール
    /// </summary>
    class LuaModule final : public Module::ModuleBase
    {
        HE_MODULE_GENRATE_DECLARATION(LuaModule);

    public:
        LuaModule() : ModuleBase(ModuleName()) {}

    protected:
        /// <summary>
        /// モジュール初期化
        /// </summary>
        Bool _VStart() override final;
        /// <summary>
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        Bool _VRelease() override final;

    private:
        void* _pLuaState = NULL;
    };
}  // namespace Lua
