#pragma once

#include "Engine/Common/CustomMap.h"
#include "Engine/Common/Function.h"
#include "Engine/Common/PoolManager.h"
#include "Engine/Module/Module.h"

// エンジンの最小インクルード
#include "Engine/MiniEngine.h"

// モジュールのヘッダーファイルは全てインクルードする
namespace Lua
{
    enum ELuaFuncArgType
    {
        ELuaFuncArgType_Float32 = 0,
        ELuaFuncArgType_Str
    };

    struct LuaFuncArgData
    {
        ELuaFuncArgType eValType;

        union
        {
            Float32 fVal;
            Char szText[128];
        } data;
    };

    /// <summary>
    /// Luaスクリプトの関数結果
    /// </summary>
    struct LuaFuncData
    {
        // 関数名
        Char szFuncName[128] = {NULL};
        // 各引数
        // 引数名は取れないので, 使う側が配列の要素位置を意識する必要がある
        LuaFuncArgData aArg[6];

        Uint32 uArgCount = 0;
    };

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
        /// Luaスクリプトのコルーチンは非対応
        /// </summary>
        template <typename... Args>
        Bool CallScriptFunc(const Core::Common::Handle&, const Char*, Args... args);

        // TODO: c++側がキャッチできる関数を登録
        Bool RegistScriptFunc(const Core::Common::Handle&, const Char*);

        /// <summary>
        /// 登録した関数をLuaスクリプトで実行を受け取る関数を設定
        /// </summary>
        Bool SetEventFunctionByLuaFunc(
            Core::Memory::SharedPtr<Core::Common::FunctionObject<void, LuaFuncData&>>);

    protected:
        /// <summary>
        /// モジュール初期化
        /// </summary>
        Bool _VStart() override final;
        /// <summary>
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        Bool _VRelease() override final;

        /// <summary>
        /// モジュール後更新
        /// </summary>
        Bool _VLateUpdate(const Float32 in_fDeltaTime) override final;

    private:
        template <typename... Args>
        void _LuaStackPushArguments(void* in_pLuaState, Args&&... args)
        {
            (this->_LuaStackPushValue(in_pLuaState, std::forward<Args>(args)), ...);
        }

        void _LuaStackPushArguments(void*)
        {
            // 何もしない
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
        Core::Common::CustomFixMap<void*, Core::Common::Handle, 128> _mUseLuaObject;

        Core::Common::CustomFixMap<
            std::uintptr_t,
            Core::Memory::SharedPtr<Core::Common::FunctionObject<void, LuaFuncData&>>, 128>
            _mScriptFuncAction;
    };

    /// <summary>
    /// Luaスクリプトの関数を呼び出す
    /// Luaスクリプトのコルーチンは使えない
    /// </summary>
    template <typename... Args>
    Bool LuaModule::CallScriptFunc(const Core::Common::Handle& in_rHandle,
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
