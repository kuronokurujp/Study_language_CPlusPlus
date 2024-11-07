#include "LuaModule.h"

#include "Engine/Memory/Memory.h"
#include "Lua/ThirdParty/lua/src/lua.hpp"

namespace Lua
{

    static UTF8 s_szPushStackTempText[1024] = {NULL};

    static void _AddFuncByState(lua_State* in_pState, const Char* in_szFuncName,
                                lua_CFunction in_FuncAddr)
    {
        Core::Common::s_szTempFixString256 = in_szFuncName;

        UTF8 szRegistFuncName[256] = {NULL};
        Core::Common::s_szTempFixString256
            .OutputUTF8(szRegistFuncName, Core::Common::s_szTempFixString256.Capacity());

        lua_register(in_pState, szRegistFuncName, in_FuncAddr);
    }

    /// <summary>
    /// Luaスクリプト内で呼び出されるログ関数
    /// </summary>
    static int _LuaFuncByLog(lua_State* in_pLuaState)
    {
#ifdef _DEBUG
        if (in_pLuaState == NULL)
        {
            return 0;
        }

        const UTF8* pText = NULL;
        if (lua_gettop(in_pLuaState) == 1)
        {
            pText = lua_tostring(in_pLuaState, 1);
            if (pText == NULL)
            {
                Core::Common::s_szTempFixString1024 = lua_tostring(in_pLuaState, -1);
                HE_LOG_LINE(HE_STR_TEXT("Error, %s"), Core::Common::s_szTempFixString1024.Str());
            }
        }
        else
        {
            HE_LOG_LINE(HE_STR_TEXT("設定した引数が多いです"));
        }

        if (pText)
        {
            Core::Common::s_szTempFixString1024 = pText;
            HE_LOG_LINE(Core::Common::s_szTempFixString1024.Str());

            lua_settop(in_pLuaState, 0);
        }
        else
        {
            lua_settop(in_pLuaState, 1);
        }

#endif
        return 0;
    }

    // カスタムメモリアロケータ関数
    static void* _LuaAllocator(void* ud, void* ptr, size_t osize, size_t nsize)
    {
        // メモリ解放
        if (nsize <= 0)
        {
            // メモリの解放
            HE_SAFE_DELETE_MEM(ptr);
            return NULL;
        }
        // メモリの新規割り当て
        else if (ptr == NULL)
        {
            return HE_ALLOC_MEM(nsize, 0);
        }
        // メモリの割り当て変更
        else
        {
            // メモリの再割り当て
            // メモリの拡張・伸縮機能がないので新しく確保
            // メモリ内容をコピーして古いのは解放する
            void* p = HE_ALLOC_MEM(nsize, 0);
            ::memcpy_s(p, nsize, ptr, HE_MIN(osize, nsize));

            HE_SAFE_DELETE_MEM(ptr);
            return p;
        }
    }

    static void _LuaErrorMsg(lua_State* in_pLuaState, const int in_iErrorCode)
    {
#ifdef _DEBUG
        HE_ASSERT(in_pLuaState);

        switch (in_iErrorCode)
        {
            case LUA_OK:
                HE_LOG_LINE(HE_STR_TEXT("エラーなし"));
                break;
            case LUA_ERRRUN:
                HE_LOG_LINE(HE_STR_TEXT("ランタイムエラー"));
                break;
            case LUA_ERRSYNTAX:
                HE_LOG_LINE(HE_STR_TEXT("コンパイル前の構文エラー"));
                break;
            case LUA_ERRMEM:
                HE_LOG_LINE(HE_STR_TEXT("メモリ割り当て（メモリ外）エラー"));
                break;
            case LUA_ERRERR:
                HE_LOG_LINE(HE_STR_TEXT("メッセージハンドラー実行中のエラー"));
                break;
        }
#endif
    }

    // Sint32型をプッシュ
    static void pushValue(lua_State* L, const Sint32 value)
    {
        lua_pushinteger(L, static_cast<lua_Integer>(value));
    }

    // Uint32型をプッシュ
    static void pushValue(lua_State* L, const Uint32 value)
    {
        lua_pushinteger(L, static_cast<lua_Integer>(value));
    }

    // Float32
    static void pushValue(lua_State* L, const Float32 value)
    {
        lua_pushnumber(L, static_cast<lua_Number>(value));
    }

    // Bool型をプッシュ
    static void pushValue(lua_State* L, const Bool value)
    {
        lua_pushboolean(L, value);
    }

    // const Char* 型をプッシュ
    static void pushValue(lua_State* L, const Char* value)
    {
    }

    // StringBase型を
    static void pushValue(lua_State* L, const Core::Common::StringBase& value)
    {
    }

    // ベースケース：引数がない場合
    static void pushArguments(lua_State* L)
    {
        // 何もしない
    }

    // 再帰ケース：少なくとも一つの引数がある場合
    template <typename T, typename... Args>
    static void pushArguments(lua_State* L, T value, Args... args)
    {
        pushValue(L, value);        // 現在の引数をプッシュ
        pushArguments(L, args...);  // 残りの引数を処理（再帰）
    }

    const Core::Common::Handle LuaModule::CreateLuaObject(const Char* in_pName)
    {
        HE_ASSERT(in_pName && "Luaオブジェクトの作成で名前指定がない");

        Core::Common::s_szTempFixString128 = in_pName;

        Core::Common::Handle handle;
        auto* pLuaObject = this->_luaObjectPool.Alloc(&handle);

        pLuaObject->pLuaState = lua_newstate(_LuaAllocator, NULL);
        HE_ASSERT(pLuaObject->pLuaState);

        lua_State* pLuaState = reinterpret_cast<lua_State*>(pLuaObject->pLuaState);
        luaL_openlibs(pLuaState);

        //	常に常駐させる関数登録
        {
            //	デバッグ表示
            _AddFuncByState(pLuaState, HE_STR_TEXT("HE_LOG"), &_LuaFuncByLog);
        }

        HE_STR_CPY_S(pLuaObject->szName, HE_ARRAY_NUM(pLuaObject->szName),
                     Core::Common::s_szTempFixString128.Str(),
                     Core::Common::s_szTempFixString128.Length());

        this->_mUseLuaObject.Add(pLuaObject, handle);

        return handle;
    }

    Bool LuaModule::ReleaseLuaObject(const Core::Common::Handle& in_rHandle)
    {
        HE_ASSERT(in_rHandle.Null() == FALSE);

        auto* pLuaObject = this->_luaObjectPool.Ref(in_rHandle);
        if (pLuaObject == NULL) return FALSE;

        this->_mUseLuaObject.Erase(pLuaObject);

        lua_State* pLuaState = reinterpret_cast<lua_State*>(pLuaObject->pLuaState);
        HE_ASSERT(pLuaState);
        lua_close(pLuaState);

        return this->_luaObjectPool.Free(in_rHandle);
    }

    Bool LuaModule::LoadScriptText(const Core::Common::Handle& in_rHandle, const Char* in_pText)
    {
        HE_ASSERT(in_rHandle.Null() == FALSE);

        Core::Common::s_szTempFixString1024 = in_pText;
        HE_ASSERT(0 < Core::Common::s_szTempFixString1024.Length());

        auto* pLuaObject = this->_luaObjectPool.Ref(in_rHandle);
        if (pLuaObject == NULL) return FALSE;

        UTF8 szText[1024] = {NULL};
        Core::Common::s_szTempFixString1024.OutputUTF8(szText, HE_ARRAY_NUM(szText));

        lua_State* pLuaState = reinterpret_cast<lua_State*>(pLuaObject->pLuaState);
        HE_ASSERT(pLuaState);
        auto result = luaL_dostring(pLuaState, szText);
        if (result == LUA_OK)
        {
            return TRUE;
        }

        // TODO: エラー処理
        _LuaErrorMsg(pLuaState, result);
        return FALSE;
    }

    /// <summary>
    /// モジュール初期化
    /// </summary>
    Bool LuaModule::_VStart()
    {
        HE_ASSERT(this->_mUseLuaObject.Empty());
        return TRUE;
    }

    /// <summary>
    /// インスタンス破棄時に呼ばれる
    /// </summary>
    Bool LuaModule::_VRelease()
    {
        while (this->_mUseLuaObject.Empty() == FALSE)
        {
            auto itr = this->_mUseLuaObject.Begin();
            this->ReleaseLuaObject(itr->data);
        }
        HE_ASSERT(this->_mUseLuaObject.Empty());

        return TRUE;
    }

    // Int型をプッシュ
    void LuaModule::_LuaStackPushValue(void* in_pLuaState, const Int in_value)
    {
        lua_State* pLuaState = reinterpret_cast<lua_State*>(in_pLuaState);
        HE_ASSERT(pLuaState);

        lua_pushinteger(pLuaState, static_cast<lua_Integer>(in_value));
    }

    // Sint32型をプッシュ
    void LuaModule::_LuaStackPushValue(void* in_pLuaState, const Sint32 in_iValue)
    {
        lua_State* pLuaState = reinterpret_cast<lua_State*>(in_pLuaState);
        HE_ASSERT(pLuaState);

        lua_pushinteger(pLuaState, static_cast<lua_Integer>(in_iValue));
    }

    // Uint32型をプッシュ
    void LuaModule::_LuaStackPushValue(void* in_pLuaState, const Uint32 in_uValue)
    {
        lua_State* pLuaState = reinterpret_cast<lua_State*>(in_pLuaState);
        HE_ASSERT(pLuaState);

        lua_pushinteger(pLuaState, static_cast<lua_Integer>(in_uValue));
    }

    // Float32
    void LuaModule::_LuaStackPushValue(void* in_pLuaState, const Float32 in_fValue)
    {
        lua_State* pLuaState = reinterpret_cast<lua_State*>(in_pLuaState);
        HE_ASSERT(pLuaState);

        lua_pushnumber(pLuaState, static_cast<lua_Number>(in_fValue));
    }

    // Bool型をプッシュ
    void LuaModule::_LuaStackPushValue(void* in_pLuaState, const Bool in_bValue)
    {
        lua_State* pLuaState = reinterpret_cast<lua_State*>(in_pLuaState);
        HE_ASSERT(pLuaState);
        lua_pushboolean(pLuaState, in_bValue);
    }

    // const Char* 型をプッシュ
    void LuaModule::_LuaStackPushValue(void* in_pLuaState, const Char* in_szValue)
    {
        lua_State* pLuaState = reinterpret_cast<lua_State*>(in_pLuaState);
        HE_ASSERT(pLuaState);

        Core::Common::s_szTempFixString1024 = in_szValue;
        Core::Common::s_szTempFixString1024.OutputUTF8(s_szPushStackTempText,
                                                       HE_ARRAY_NUM(s_szPushStackTempText));

        lua_pushstring(pLuaState, s_szPushStackTempText);
    }

    // StringBase型を
    void LuaModule::_LuaStackPushValue(void* in_pLuaState,
                                       const Core::Common::StringBase& in_rszValue)
    {
        lua_State* pLuaState = reinterpret_cast<lua_State*>(in_pLuaState);
        HE_ASSERT(pLuaState);

        in_rszValue.OutputUTF8(s_szPushStackTempText, HE_ARRAY_NUM(s_szPushStackTempText));
        size_t size = HE_MIN(HE_ARRAY_NUM(s_szPushStackTempText), in_rszValue.Length());

        lua_pushlstring(pLuaState, s_szPushStackTempText, size);
    }

    Bool LuaModule::_BeginLocalFunc(void* in_pLuaState, const Char* in_szFuncName)
    {
        lua_State* pLuaState = reinterpret_cast<lua_State*>(in_pLuaState);
        HE_ASSERT(pLuaState);

        // 実行するLua関数名をスタックにプッシュ
        Core::Common::s_szTempFixString1024 = in_szFuncName;
        Core::Common::s_szTempFixString1024.OutputUTF8(s_szPushStackTempText,
                                                       HE_ARRAY_NUM(s_szPushStackTempText));

        // 名前が関数名でなければエラー
        if (lua_getglobal(pLuaState, s_szPushStackTempText) != LUA_TFUNCTION)
        {
            return FALSE;
        }

        if (lua_isfunction(pLuaState, -1) == FALSE)
        {
            HE_LOG_LINE(HE_STR_TEXT("Lua関数(%s)が存在しない"), s_szPushStackTempText);
            lua_pop(pLuaState, 1);

            return FALSE;
        }

        return TRUE;
    }

    Bool LuaModule::_EndLocalFunc(void* in_pLuaState, const Uint32 in_uArgCount)
    {
        lua_State* pLuaState = reinterpret_cast<lua_State*>(in_pLuaState);
        HE_ASSERT(pLuaState);

        // TODO: Lua関数の戻り値は非対応
        if (lua_pcall(pLuaState, in_uArgCount, 0, 0) != LUA_OK)
        {
            const UTF8* szError = lua_tolstring(pLuaState, -1, NULL);
            HE_LOG_LINE(HE_STR_TEXT("エラー: Lua関数の呼び出しに失敗\nエラー内容: %s"), szError);
            // エラーメッセージをポップ
            lua_pop(pLuaState, 1);

            return FALSE;
        }

        return TRUE;
    }

}  // namespace Lua
