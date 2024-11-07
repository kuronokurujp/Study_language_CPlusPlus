#pragma once

#include "Engine/Common/CustomMap.h"
#include "Engine/Common/PoolManager.h"
#include "Engine/Module/Module.h"

// エンジンの最小インクルード
#include "Engine/MiniEngine.h"

// モジュールのヘッダーファイルは全てインクルードする
namespace Lua
{
    /// <summary>
    /// Luaアクター用の追加モジュール
    /// </summary>
    class LuaModule final : public Module::ModuleBase
    {
        HE_MODULE_GENRATE_DECLARATION(LuaModule);

    private:
        struct LuaObject
        {
            Char szName[128] = {NULL};
            void* pLuaState  = NULL;
        };

    public:
        LuaModule() : ModuleBase(ModuleName()) {}

        /// <summary>
        /// Luaオブジェクト生成
        /// </summary>
        const Core::Common::Handle CreateLuaObject(const Char* in_pName);

        /// <summary>
        /// Luaオブジェクトを破棄
        /// </summary>
        Bool ReleaseLuaObject(const Core::Common::Handle&);

        /// <summary>
        /// Luaスクリプトテキストをロード
        /// </summary>
        Bool LoadScriptText(const Core::Common::Handle&, const Char*);

        /// <summary>
        /// Luaスクリプトのローカル関数を呼び出す
        /// 関数の引数は可変指定
        /// TODO: ローカル関数の戻り値は非対応
        /// </summary>
        template <typename... Args>
        Bool CallScriptLocalFunc(const Core::Common::Handle&, const Char*, Args... args);

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
        void _LuaStackPushArguments(void*)
        {
            // 何もしない
        }

        template <typename T, typename... Args>
        void _LuaStackPushArguments(void* L, T value, Args... args)
        {
            this->_LuaStackPushValue(L, value);        // 現在の引数をプッシュ
            this->_LuaStackPushArguments(L, args...);  // 残りの引数を処理
        }

        // Int型をプッシュ
        void _LuaStackPushValue(void*, const Int);

        // Sint32型をプッシュ
        void _LuaStackPushValue(void*, const Sint32);

        // Uint32型をプッシュ
        void _LuaStackPushValue(void*, const Uint32);

        // Float32
        void _LuaStackPushValue(void*, const Float32);

        // Bool型をプッシュ
        void _LuaStackPushValue(void*, const Bool);

        // const Char* 型をプッシュ
        void _LuaStackPushValue(void*, const Char*);

        // StringBase型を
        void _LuaStackPushValue(void*, const Core::Common::StringBase&);

        Bool _BeginLocalFunc(void*, const Char*);
        Bool _EndLocalFunc(void*, const Uint32);

    private:
        void* _pLuaState = NULL;
        Core::Common::FixPoolManager<LuaObject, 128> _luaObjectPool;
        Core::Common::CustomFixMap<LuaObject*, Core::Common::Handle, 128> _mUseLuaObject;
    };

    template <typename... Args>
    Bool LuaModule::CallScriptLocalFunc(const Core::Common::Handle& in_rHandle,
                                        const Char* in_pActionName, Args... args)
    {
        auto* pLuaObject = this->_luaObjectPool.Ref(in_rHandle);
        if (this->_BeginLocalFunc(pLuaObject->pLuaState, in_pActionName) == FALSE) return FALSE;

        Uint32 uArgCount = sizeof...(args);
        this->_LuaStackPushArguments(pLuaObject->pLuaState, args...);

        if (this->_EndLocalFunc(pLuaObject->pLuaState, uArgCount) == FALSE)
        {
            return FALSE;
        }

        return TRUE;
    }
}  // namespace Lua
