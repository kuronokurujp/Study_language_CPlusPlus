#include "LuaModule.h"

#include "Engine/Common/CustomStack.h"
#include "Engine/Memory/Memory.h"

// Luaエンジン
#include "../ThirdParty/lua/src/lua.hpp"

namespace Lua
{
    namespace Local
    {
        static HE::UTF8 _szPushStackTempText[1024] = HE_STR_EMPTY;

        // Luaモジュールのインスタンスは一つしかない前提
        static Core::Common::FixedStack<LuaFuncData, 128> _sLuaFuncResultData;

        /// <summary>
        /// Lua処理でエラーが起きた場合のトレースバックのデバッグ処理
        /// </summary>
        int _LuaDebugTraceback(lua_State* L)
        {
            const HE::UTF8* msg = lua_tostring(L, 1);
            if (msg)
            {
                luaL_traceback(L, L, msg, 1);
            }
            else
            {
                lua_pushliteral(L, "No error message");
            }

            return 1;
        }

        /// <summary>
        /// Luaスクリプトで実行する関数を追加
        /// </summary>
        static HE::Bool _LuaRegistFuncByState(lua_State* in_pState, const HE::Char* in_szFuncName,
                                              lua_CFunction in_funcAddr)
        {
            Core::Common::g_szTempFixedString256 = in_szFuncName;

            HE::UTF8 szRegistFuncName[256] = HE_STR_EMPTY;
            Core::Common::g_szTempFixedString256
                .OutputUTF8(szRegistFuncName, Core::Common::g_szTempFixedString256.Capacity());

            // アップバリューとして関数名をプッシュ
            lua_pushstring(in_pState, szRegistFuncName);
            // アップバリューを1つ持つクロージャを作成
            lua_pushcclosure(in_pState, in_funcAddr, 1);
            // グローバル関数として登録
            lua_setglobal(in_pState, szRegistFuncName);

            return TRUE;
        }

        /// <summary>
        /// Luaスクリプト内で呼び出される関数
        /// </summary>
        static int _LuaScriptCallFunc(lua_State* in_pLuaState)
        {
            // 関数結果をスタックにプッシュ
            LuaFuncData* pFuncData = _sLuaFuncResultData.PushFront();

            // 関数名を取得
            {
                const HE::UTF8* szName = lua_tostring(in_pLuaState, lua_upvalueindex(1));
                // 関数名設定
                Core::Common::g_szTempFixedString128 = szName;
                HE_STR_COPY_S(pFuncData->szFuncName, HE_ARRAY_NUM(pFuncData->szFuncName),
                              Core::Common::g_szTempFixedString128.Str(),
                              Core::Common::g_szTempFixedString128.Size());
            }

            // 関数の引数の数を取得
            const HE::Uint32 argCount = static_cast<HE::Uint32>(lua_gettop(in_pLuaState));
            HE_ASSERT(argCount <= HE_ARRAY_NUM(pFuncData->aArg));

            // スタックにプッシュしたデータをうめる
            pFuncData->uArgCount = argCount;

            // TODO: 非型テンプレートで一行でCallを呼び出すようにする
            // 引数の各値を取得
            for (HE::Uint32 i = 1; i <= argCount; ++i)
            {
                auto* pArgData = &pFuncData->aArg[i - 1];
                int type       = lua_type(in_pLuaState, i);
                switch (type)
                {
                    case LUA_TNUMBER:
                    {
                        if (lua_isinteger(in_pLuaState, i))
                        {
                            pArgData->eValType     = ELuaFuncArgType_Uint64;
                            const HE::Uint64 uuVal = lua_tointeger(in_pLuaState, i);
                            pArgData->_data.uuVal  = uuVal;
                        }
                        else
                        {
                            pArgData->eValType     = ELuaFuncArgType_Float32;
                            const HE::Float32 fVal = lua_tonumber(in_pLuaState, i);
                            pArgData->_data.fVal   = fVal;
                        }

                        break;
                    }
                    case LUA_TSTRING:
                    {
                        pArgData->eValType = ELuaFuncArgType_Str;

                        const HE::UTF8* pStr                 = lua_tostring(in_pLuaState, i);
                        Core::Common::g_szTempFixedString128 = pStr;

                        HE_STR_COPY_S(pArgData->_data.szText, HE_ARRAY_NUM(pArgData->_data.szText),
                                      Core::Common::g_szTempFixedString128.Str(),
                                      Core::Common::g_szTempFixedString128.Size());

                        break;
                    }
                    // 非対応の型なのでエラー
                    default:
                    {
                        HE_ASSERT(FALSE && "Luaスクリプトが呼び出した関数の引数型が対応していない");
                        break;
                    }
                }
            }

            return 0;
        }

#ifdef _DEBUG
        /// <summary>
        /// Luaスクリプト内で呼び出されるログ関数
        /// </summary>
        static int _LuaFuncByLog(lua_State* in_pLuaState)
        {
            if (in_pLuaState == NULL)
            {
                return 0;
            }

            const HE::UTF8* pText = NULL;
            if (lua_gettop(in_pLuaState) == 1)
            {
                pText = lua_tostring(in_pLuaState, 1);
                if (pText == NULL)
                {
                    Core::Common::g_szTempFixedString1024 = lua_tostring(in_pLuaState, -1);
                    HE_LOG_LINE(HE_STR_TEXT("Error, %s"),
                                Core::Common::g_szTempFixedString1024.Str());
                }
            }
            else
            {
                HE_LOG_LINE(HE_STR_TEXT("設定した引数が多いです"));
            }

            if (pText)
            {
                Core::Common::g_szTempFixedString1024 = pText;
                HE_LOG_LINE(Core::Common::g_szTempFixedString1024.Str());

                lua_settop(in_pLuaState, 0);
            }
            else
            {
                lua_settop(in_pLuaState, 1);
            }

            return 0;
        }
#endif

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

#ifdef _DEBUG
        static void _LuaErrorMsg(lua_State* in_pLuaState, const int in_iErrorCode)
        {
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
        }
#endif
    }  // namespace Local

    const Core::Common::Handle LuaModule::CreateLuaObject(const HE::Char* in_pName)
    {
        HE_ASSERT(in_pName && "Luaオブジェクトの作成で名前指定がない");

        // TODO: スレッドを使った非同期対応が必要
        Core::Common::g_szTempFixedString128 = in_pName;

        // Core::Common::Handle handle;
        auto [handle, pLuaObject] = this->_luaObjectPool.Alloc();  //&handle);

        pLuaObject->pLuaState = lua_newstate(Local::_LuaAllocator, NULL);
        HE_ASSERT(pLuaObject->pLuaState);

        lua_State* pLuaState = reinterpret_cast<lua_State*>(pLuaObject->pLuaState);
        luaL_openlibs(pLuaState);

        //	常に常駐させる関数登録
        {
#ifdef _DEBUG
            //	デバッグ表示
            Local::_LuaRegistFuncByState(pLuaState, HE_STR_TEXT("HE_LOG"), &Local::_LuaFuncByLog);
#endif
        }

        HE_STR_COPY_S(pLuaObject->szName, HE_ARRAY_NUM(pLuaObject->szName),
                      Core::Common::g_szTempFixedString128.Str(),
                      Core::Common::g_szTempFixedString128.Size());

        this->_mUseLuaObject.Add(pLuaState, handle);

        return handle;
    }

    HE::Bool LuaModule::ReleaseLuaObject(Core::Common::Handle& in_rHandle)
    {
        HE_ASSERT(in_rHandle.Null() == FALSE);

        auto* pLuaObject = this->_luaObjectPool.Ref(in_rHandle);
        if (pLuaObject == NULL) return FALSE;

        lua_State* pLuaState = reinterpret_cast<lua_State*>(pLuaObject->pLuaState);
        HE_ASSERT(pLuaState);
        lua_close(pLuaState);

        this->_mUseLuaObject.Erase(pLuaObject->pLuaState);

        return this->_luaObjectPool.Free(in_rHandle);
    }

    HE::Bool LuaModule::LoadScriptText(const Core::Common::Handle& in_rHandle,
                                       const HE::Char* in_pText)
    {
        HE_ASSERT(in_rHandle.Null() == FALSE);

        Core::Common::g_szTempFixedString1024 = in_pText;
        HE_ASSERT(0 < Core::Common::g_szTempFixedString1024.Size());

        auto* pLuaObject = this->_luaObjectPool.Ref(in_rHandle);
        if (pLuaObject == NULL) return FALSE;

        HE::UTF8 szText[1024] = HE_STR_EMPTY;
        Core::Common::g_szTempFixedString1024.OutputUTF8(szText, HE_ARRAY_NUM(szText));

        lua_State* pLuaState = reinterpret_cast<lua_State*>(pLuaObject->pLuaState);
        HE_ASSERT(pLuaState);
        // Luaコードをロード
        if (luaL_loadstring(pLuaState, szText) != LUA_OK)
        {
            // スタックトップのエラーメッセージを取得
            const HE::UTF8* szError = lua_tostring(pLuaState, -1);
            HE_LOG_LINE(HE_STR_TEXT("Lua load error: %s"), szError);

            // スタックトップのエラーメッセージをスタックから取り除く
            lua_pop(pLuaState, 1);

            return FALSE;
        }

        // エラーハンドラ関数をスタックにプッシュ
        lua_pushcfunction(pLuaState, Local::_LuaDebugTraceback);

        // スタックの並びを調整
        // 以下のスタック状態になる
        // [関数]
        // [エラーハンドラ]
        lua_insert(pLuaState, -2);

        // Luaコードを実行（エラーハンドラを指定）
        if (lua_pcall(pLuaState, 0, LUA_MULTRET, -2) == LUA_OK)
        {
            // エラーハンドラをスタックから取り除く
            lua_pop(pLuaState, 1);
            return TRUE;
        }
        else
        {
            // エラーメッセージとエラーハンドラをスタックから取り除く
            const HE::UTF8* szError = lua_tostring(pLuaState, -1);
            HE_LOG_LINE(HE_STR_TEXT("Lua runtime error:\n%s"), szError);
            lua_pop(pLuaState, 2);
        }

        return FALSE;
    }

    // c++側がキャッチできる関数を登録
    HE::Bool LuaModule::RegistScriptFunc(const Core::Common::Handle& in_rHandle,
                                         const HE::Char* in_pFuncName)
    {
        HE_ASSERT_RETURN_VALUE(FALSE, in_rHandle.Null() == FALSE);

        // Luaスクリプトを呼ぶ関数名を生成
        Core::Common::g_szTempFixedString1024 = in_pFuncName;

        HE_ASSERT_RETURN_VALUE(FALSE, 0 < Core::Common::g_szTempFixedString1024.Length());

        auto* pLuaObject = this->_luaObjectPool.Ref(in_rHandle);
        if (pLuaObject == NULL) return FALSE;

        lua_State* pLuaState = reinterpret_cast<lua_State*>(pLuaObject->pLuaState);
        HE_ASSERT_RETURN_VALUE(FALSE, pLuaState);

        // Luaスクリプトが呼び出した関数を受け取るようにする
        return Local::_LuaRegistFuncByState(pLuaState, Core::Common::g_szTempFixedString1024.Str(),
                                            &Local::_LuaScriptCallFunc);
    }

    const Core::Common::Handle LuaModule::AddListenerLuaFunc(ListenLuaFuncEventUniqutPtr in_spFunc)
    {
        // TODO: 削除する手段を返さないとだめ
        auto [handle, spFunc] = this->_poolListenLuaFuncEvent.Alloc();
        *spFunc               = std::move(in_spFunc);

        return handle;
    }

    /// <summary>
    /// モジュール初期化
    /// </summary>
    HE::Bool LuaModule::_VStart()
    {
        HE_ASSERT(this->_mUseLuaObject.Empty());

        Local::_sLuaFuncResultData.Clear();
        return TRUE;
    }

    /// <summary>
    /// インスタンス破棄時に呼ばれる
    /// </summary>
    HE::Bool LuaModule::_VRelease()
    {
        while (this->_mUseLuaObject.Empty() == FALSE)
        {
            auto itr = this->_mUseLuaObject.Begin();
            this->ReleaseLuaObject(itr->_data);
        }
        HE_ASSERT(this->_mUseLuaObject.Empty());

        // 参照先をNULLにしないとメモリが残る
        auto scriptFuncActionMap = this->_poolListenLuaFuncEvent.GetUseDataMap();
        for (auto itr = scriptFuncActionMap.Begin(); itr != scriptFuncActionMap.End(); ++itr)
        {
            Core::Common::Handle handle = itr->_key;
            this->_poolListenLuaFuncEvent.Free(handle);
        }

        Local::_sLuaFuncResultData.Clear();

        return TRUE;
    }

    void LuaModule::_VLateUpdate(const HE::Float32 in_fDeltaTime)
    {
        // Luaスクリプトから呼び出した関数結果を出力
        while (Local::_sLuaFuncResultData.Empty() == FALSE)
        {
            auto& rData = Local::_sLuaFuncResultData.PopBack();
            // 関数結果を受け取っている関数を呼び出す
            auto scriptFuncActionMap = this->_poolListenLuaFuncEvent.GetUseDataMap();
            for (auto itr = scriptFuncActionMap.Begin(); itr != scriptFuncActionMap.End(); ++itr)
            {
                itr->_data->get()->CallByRef(rData);
            }
        }
    }

    // HE::Int型をプッシュ
    void LuaModule::_LuaStackPushValue(void* in_pLuaState, const HE::Int in_value)
    {
        lua_State* pLuaState = reinterpret_cast<lua_State*>(in_pLuaState);
        HE_ASSERT(pLuaState);

        lua_pushinteger(pLuaState, static_cast<lua_Integer>(in_value));
    }

    // HE::Sint32型をプッシュ
    void LuaModule::_LuaStackPushValue(void* in_pLuaState, const HE::Sint32 in_iValue)
    {
        lua_State* pLuaState = reinterpret_cast<lua_State*>(in_pLuaState);
        HE_ASSERT(pLuaState);

        lua_pushinteger(pLuaState, static_cast<lua_Integer>(in_iValue));
    }

    // HE:Uint32型をプッシュ
    void LuaModule::_LuaStackPushValue(void* in_pLuaState, const HE::Uint32 in_uValue)
    {
        lua_State* pLuaState = reinterpret_cast<lua_State*>(in_pLuaState);
        HE_ASSERT(pLuaState);

        lua_pushinteger(pLuaState, static_cast<lua_Integer>(in_uValue));
    }

    // HE::Float32
    void LuaModule::_LuaStackPushValue(void* in_pLuaState, const HE::Float32 in_fValue)
    {
        lua_State* pLuaState = reinterpret_cast<lua_State*>(in_pLuaState);
        HE_ASSERT(pLuaState);

        lua_pushnumber(pLuaState, static_cast<lua_Number>(in_fValue));
    }

    void LuaModule::_LuaStackPushValue(void* in_pLuaState, const HE::Uint64 in_uuValue)
    {
        lua_State* pLuaState = reinterpret_cast<lua_State*>(in_pLuaState);
        HE_ASSERT(pLuaState);

        lua_pushnumber(pLuaState, static_cast<lua_Integer>(in_uuValue));
    }

    // HE::Bool型をプッシュ
    void LuaModule::_LuaStackPushValue(void* in_pLuaState, const HE::Bool in_bValue)
    {
        lua_State* pLuaState = reinterpret_cast<lua_State*>(in_pLuaState);
        HE_ASSERT(pLuaState);
        lua_pushboolean(pLuaState, in_bValue);
    }

    // const Char* 型をプッシュ
    void LuaModule::_LuaStackPushValue(void* in_pLuaState, const HE::Char* in_szValue)
    {
        lua_State* pLuaState = reinterpret_cast<lua_State*>(in_pLuaState);
        HE_ASSERT(pLuaState);

        Core::Common::g_szTempFixedString1024 = in_szValue;
        Core::Common::g_szTempFixedString1024.OutputUTF8(Local::_szPushStackTempText,
                                                         HE_ARRAY_NUM(Local::_szPushStackTempText));

        lua_pushstring(pLuaState, Local::_szPushStackTempText);
    }

    void LuaModule::_LuaStackPushValue(void* in_pLuaState,
                                       const Core::Common::StringBase& in_rszValue)
    {
        lua_State* pLuaState = reinterpret_cast<lua_State*>(in_pLuaState);
        HE_ASSERT(pLuaState);

        in_rszValue.OutputUTF8(Local::_szPushStackTempText,
                               HE_ARRAY_NUM(Local::_szPushStackTempText));
        size_t size = HE_MIN(HE_ARRAY_NUM(Local::_szPushStackTempText), in_rszValue.Size());

        lua_pushlstring(pLuaState, Local::_szPushStackTempText, size);
    }

    HE::Bool LuaModule::_BeginLocalFunc(void* in_pLuaState, const HE::Char* in_szFuncName)
    {
        lua_State* pLuaState = reinterpret_cast<lua_State*>(in_pLuaState);
        HE_ASSERT(pLuaState);

        // 実行するLua関数名をスタックにプッシュ
        Core::Common::g_szTempFixedString1024 = in_szFuncName;
        Core::Common::g_szTempFixedString1024.OutputUTF8(Local::_szPushStackTempText,
                                                         HE_ARRAY_NUM(Local::_szPushStackTempText));

        // 名前が関数名でなければエラー
        if (lua_getglobal(pLuaState, Local::_szPushStackTempText) != LUA_TFUNCTION)
        {
            HE_LOG_LINE(HE_STR_TEXT("Lua関数(%s)は関数じゃない"), Local::_szPushStackTempText);
            return FALSE;
        }

        if (lua_isfunction(pLuaState, -1) == FALSE)
        {
            HE_LOG_LINE(HE_STR_TEXT("Lua関数(%s)が存在しない"), Local::_szPushStackTempText);
            lua_pop(pLuaState, 1);

            return FALSE;
        }

        return TRUE;
    }

    HE::Bool LuaModule::_EndLocalFunc(void* in_pLuaState, const HE::Uint32 in_uArgCount)
    {
        lua_State* pLuaState = reinterpret_cast<lua_State*>(in_pLuaState);
        HE_ASSERT(pLuaState);

        // TODO: Lua関数の戻り値は非対応
        // TODO: Lua関数のコルーチンは非対応
        if (lua_pcall(pLuaState, in_uArgCount, 0, 0) != LUA_OK)
        {
            const HE::UTF8* szError = lua_tolstring(pLuaState, -1, NULL);
            HE_LOG_LINE(HE_STR_TEXT("エラー: Lua関数の呼び出しに失敗\nエラー内容: %s"), szError);
            // エラーメッセージをポップ
            lua_pop(pLuaState, 1);

            return FALSE;
        }

        return TRUE;
    }

}  // namespace Lua
