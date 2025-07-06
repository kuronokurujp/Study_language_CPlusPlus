#include <gtest/gtest.h>

#include "Engine/MiniEngine.h"
#include "LuaModule.h"

namespace Lua
{
    TEST(HobbyPlugin_Lua, CallScript)
    {
        auto spLuaModule = HE_MAKE_CUSTOM_SHARED_PTR((Lua::LuaModule));

        Module::ModuleManager moduleManager;
        EXPECT_EQ(moduleManager.AddModule(spLuaModule), TRUE);
        EXPECT_EQ(moduleManager.Start(), TRUE);

        auto luaHandle = spLuaModule->CreateLuaObject(HE_STR_TEXT("Test"));
        EXPECT_EQ(luaHandle.Null(), FALSE);

        EXPECT_EQ(spLuaModule->RegistScriptFunc(luaHandle, HE_STR_TEXT("NativeFunc")), TRUE);

        HE::Bool bCalled = FALSE;
        LuaFuncData result{};
        {
            auto upFunc =
                HE_MAKE_CUSTOM_UNIQUE_PTR((Core::Common::FunctionObject<void, LuaFuncData&>),
                                          [&bCalled, &result](LuaFuncData& d)
                                          {
                                              bCalled = TRUE;
                                              result  = d;
                                          });
            spLuaModule->AddListenerLuaFunc(std::move(upFunc));
        }

        const HE::Char* szScript = HE_STR_TEXT(
            "function LuaEntry()\n"
            "    NativeFunc(11, \"hello\")\n"
            "end\n");

        EXPECT_EQ(spLuaModule->LoadScriptText(luaHandle, szScript), TRUE);
        EXPECT_EQ(spLuaModule->CallScriptFunc(luaHandle, HE_STR_TEXT("LuaEntry")), TRUE);

        moduleManager.LateUpdate(0.0f);

        EXPECT_EQ(bCalled, TRUE);
        EXPECT_EQ(HE_STR_CMP(result.szFuncName, HE_STR_TEXT("NativeFunc")), 0);
        EXPECT_EQ(result.uArgCount, 2u);
        EXPECT_EQ(result.aArg[0].eValType, Lua::ELuaFuncArgType_Uint64);
        EXPECT_EQ(result.aArg[0]._data.uuVal, 11u);
        EXPECT_EQ(result.aArg[1].eValType, Lua::ELuaFuncArgType_Str);
        EXPECT_EQ(HE_STR_CMP(result.aArg[1]._data.szText, HE_STR_TEXT("hello")), 0);

        EXPECT_EQ(spLuaModule->ReleaseLuaObject(luaHandle), TRUE);
        EXPECT_EQ(moduleManager.Release(), TRUE);
    }
}  // namespace Lua
