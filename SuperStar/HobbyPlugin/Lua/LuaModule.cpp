#include "LuaModule.h"

#include "Engine/Memory/Memory.h"

extern "C"
{
#include "Lua/ThirdParty/lua/src/lua.hpp"
}

namespace Lua
{

    // カスタムメモリアロケータ関数
    void* LuaAllocator(void* ud, void* ptr, size_t osize, size_t nsize)
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
            ::memcpy_s(p, nsize, ptr, osize);

            HE_SAFE_DELETE_MEM(ptr);
            return p;
        }
    }

    /// <summary>
    /// モジュール初期化
    /// </summary>
    Bool LuaModule::_VStart()
    {
        this->_pLuaState = lua_newstate(LuaAllocator, NULL);
        HE_ASSERT(this->_pLuaState);

        lua_State* pLuaState = reinterpret_cast<lua_State*>(this->_pLuaState);
        luaL_openlibs(pLuaState);

        return TRUE;
    }

    /// <summary>
    /// インスタンス破棄時に呼ばれる
    /// </summary>
    Bool LuaModule::_VRelease()
    {
        lua_State* pLuaState = reinterpret_cast<lua_State*>(this->_pLuaState);
        lua_close(pLuaState);

        this->_pLuaState = NULL;

        return TRUE;
    }
}  // namespace Lua
