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
        ELuaFuncArgType_Uint64,
        ELuaFuncArgType_Str
    };

    struct LuaFuncArgData
    {
        ELuaFuncArgType eValType;

        union
        {
            HE::Float32 fVal;
            HE::Uint64 uuVal;
            HE::Char szText[128];
        } _data;
    };

    /// <summary>
    /// Luaスクリプトの関数結果
    /// </summary>
    struct LuaFuncData
    {
        // 関数名
        HE::Char szFuncName[128] = HE_STR_EMPTY;
        // 各引数
        // 引数名は取れないので, 使う側が配列の要素位置を意識する必要がある
        LuaFuncArgData aArg[6];

        HE::Uint32 uArgCount = 0;
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
            HE::Char szName[128] = HE_STR_EMPTY;
            void* pLuaState      = NULL;
        };

    public:
        using ListenLuaFuncEventUniqutPtr =
            Core::Memory::UniquePtr<Core::Common::FunctionObject<void, LuaFuncData&>>;

    public:
        LuaModule() : ModuleBase(ModuleName()) {}

        /// <summary>
        /// Luaオブジェクト生成
        /// </summary>
        const Core::Common::Handle CreateLuaObject(const HE::Char* in_pName);

        /// <summary>
        /// Luaオブジェクトを破棄
        /// </summary>
        HE::Bool ReleaseLuaObject(Core::Common::Handle&);

        /// <summary>
        /// Luaスクリプトテキストをロード
        /// </summary>
        HE::Bool LoadScriptText(const Core::Common::Handle&, const HE::Char*);

        /// <summary>
        /// Luaスクリプトのローカル関数を呼び出す
        /// 関数の引数は可変指定
        /// TODO: ローカル関数の戻り値は非対応
        /// Luaスクリプトのコルーチンは非対応
        /// </summary>
        template <typename... TArgs>
        HE::Bool CallScriptFunc(const Core::Common::Handle&, const HE::Char*, TArgs... args);

        // Luaスクリプトで呼び出せる関数を登録
        HE::Bool RegistScriptFunc(const Core::Common::Handle&, const HE::Char*);

        /// <summary>
        /// Luaスクリプトでした関数を受け取るリスナー追加
        /// </summary>
        const Core::Common::Handle AddListenerLuaFunc(ListenLuaFuncEventUniqutPtr);

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
        /// モジュール後更新
        /// </summary>
        void _VLateUpdate(const HE::Float32 in_fDeltaTime) override final;

    private:
        template <typename... TArgs>
        void _LuaStackPushArguments(void* in_pLuaState, TArgs&&... args)
        {
            (this->_LuaStackPushValue(in_pLuaState, std::forward<TArgs>(args)), ...);
        }

        void _LuaStackPushArguments(void*)
        {
            // 何もしない
        }

        // HE::Int型をプッシュ
        void _LuaStackPushValue(void*, const HE::Int);

        // HE::Sint32型をプッシュ
        void _LuaStackPushValue(void*, const HE::Sint32);

        // Uint32型をプッシュ
        void _LuaStackPushValue(void*, const HE::Uint32);

        // HE::Float32
        void _LuaStackPushValue(void*, const HE::Float32);

        // HE::Uint64
        void _LuaStackPushValue(void*, const HE::Uint64);

        // HE::Bool型をプッシュ
        void _LuaStackPushValue(void*, const HE::Bool);

        // const Char* 型をプッシュ
        void _LuaStackPushValue(void*, const HE::Char*);

        // StringBase型を
        void _LuaStackPushValue(void*, const Core::Common::StringBase&);

        HE::Bool _BeginLocalFunc(void*, const HE::Char*);
        HE::Bool _EndLocalFunc(void*, const HE::Uint32);

    private:
        void* _pLuaState = NULL;
        Core::Common::FixedPoolManager<LuaObject, 128> _luaObjectPool;
        Core::Common::FixedMap<void*, Core::Common::Handle, 128> _mUseLuaObject;

        Core::Common::FixedPoolManager<ListenLuaFuncEventUniqutPtr, 128> _poolListenLuaFuncEvent;
        /*
        Core::Common::FixedMap<
            Core::Common::Handle,
            Core::Memory::SharedPtr<Core::Common::FunctionObject<void, LuaFuncData&>>, 128>
            _mScriptFuncAction;
            */
    };

    /// <summary>
    /// Luaスクリプトの関数を呼び出す
    /// Luaスクリプトのコルーチンは使えない
    /// </summary>
    template <typename... TArgs>
    HE::Bool LuaModule::CallScriptFunc(const Core::Common::Handle& in_rHandle,
                                       const HE::Char* in_pActionName, TArgs... args)
    {
        auto* pLuaObject = this->_luaObjectPool.Ref(in_rHandle);
        if (this->_BeginLocalFunc(pLuaObject->pLuaState, in_pActionName) == FALSE) return FALSE;

        HE::Uint32 uArgCount = sizeof...(args);
        this->_LuaStackPushArguments(pLuaObject->pLuaState, args...);

        if (this->_EndLocalFunc(pLuaObject->pLuaState, uArgCount) == FALSE)
        {
            return FALSE;
        }

        return TRUE;
    }
}  // namespace Lua
