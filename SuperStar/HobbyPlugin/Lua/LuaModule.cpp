#include "LuaModule.h"

#include "Lua/ThirdParty/lua/src/lua.hpp"

namespace Lua
{
    /// <summary>
    /// モジュール初期化
    /// </summary>
    Bool LuaModule::_VStart()
    {
        return TRUE;
    }

    /// <summary>
    /// インスタンス破棄時に呼ばれる
    /// </summary>
    Bool LuaModule::_VRelease()
    {
        return TRUE;
    }
}  // namespace Lua
