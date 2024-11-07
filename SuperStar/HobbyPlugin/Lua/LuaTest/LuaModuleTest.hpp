#pragma once

#include "../LuaModule.h"

#define HE_UNIT_TEST_MODE
#include "Engine/Engine.h"

TEST_CASE("Lua Module Runner")
{
    UnitTestRunnerByModuleOnly<Lua::LuaModule>(
        [](Lua::LuaModule* in_pModule)
        {
            HE_LOG_LINE(HE_STR_TEXT("Luaモジュールテストできるー"));
            auto handle = in_pModule->CreateLuaObject(HE_STR_TEXT("Test"));
            handle      = in_pModule->CreateLuaObject(HE_STR_TEXT("Test2"));
            in_pModule->ReleaseLuaObject(handle);
        });
}

TEST_CASE("Lua Module Script Func")
{
    UnitTestRunnerByModuleOnly<Lua::LuaModule>(
        [](Lua::LuaModule* in_pModule)
        {
            HE_LOG_LINE(HE_STR_TEXT("LuaモジュールのLuaスクリプトをロードしてLua関数を実行"));

            auto handle = in_pModule->CreateLuaObject(HE_STR_TEXT("Test"));
            // Luaスクリプトテキストを読み込む
            Char* szScriptText = HE_STR_TEXT(
                "function Message()\n"
                "   HE_LOG(\"田中太郎\")\n"
                "end");

            CHECK(in_pModule->LoadScriptText(handle, szScriptText));

            // TODO: Luaスクリプトの関数を実行
            CHECK(in_pModule->CallScriptLocalFunc(handle, HE_STR_TEXT("Message")));
        });
}
