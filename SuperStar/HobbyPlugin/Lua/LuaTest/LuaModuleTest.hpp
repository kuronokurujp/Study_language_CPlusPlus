#pragma once

#include "../LuaModule.h"

#define HE_UNIT_TEST_MODE
#include "Engine/Common/Function.h"
#include "Engine/Engine.h"

TEST_CASE("Lua Module Runner")
{
    UnitTestRunnerByModuleOnly<Lua::LuaModule>(
        []()
        {
            auto pModule = HE_ENGINE.ModuleManager().Get<Lua::LuaModule>();

            HE_LOG_LINE(HE_STR_TEXT("Luaモジュールテストできるー"));
            auto handle = pModule->CreateLuaObject(HE_STR_TEXT("Test"));
            handle      = pModule->CreateLuaObject(HE_STR_TEXT("Test2"));
            pModule->ReleaseLuaObject(handle);

            return TRUE;
        });
}

TEST_CASE("Lua Module Script Func")
{
    UnitTestRunnerByModuleOnly<Lua::LuaModule>(
        []()
        {
            auto pModule = HE_ENGINE.ModuleManager().Get<Lua::LuaModule>();

            HE_LOG_LINE(HE_STR_TEXT("LuaスクリプトをロードしてLua関数を実行"));

            auto handle = pModule->CreateLuaObject(HE_STR_TEXT("Test"));
            // Luaスクリプトテキストを読み込む
            Char* szScriptText = HE_STR_TEXT(
                "function Message()\n"
                "   HE_LOG(\"田中太郎\")\n"
                "end\n"
                "\n"
                "function MessageArg(in_text, in_val, in_val2, in_text2)\n"
                "   print(\"in_text: \" .. in_text)\n"
                "   print(\"in_val: \" .. in_val)\n"
                "   print(\"in_val2: \" .. in_val2)\n"
                "   print(\"in_text2: \" .. in_text2)\n"
                "end");

            CHECK(pModule->LoadScriptText(handle, szScriptText));

            // Luaスクリプトの関数を実行
            CHECK(pModule->CallScriptFunc(handle, HE_STR_TEXT("Message")));
            // 存在しない関数を実行してエラーになるか
            CHECK(pModule->CallScriptFunc(handle, HE_STR_TEXT("M")) == FALSE);

            // 引数がないLua関数にコードが引数を設定してもLua関数は正常に動作するか
            Core::Common::g_szTempFixedString16 = HE_STR_TEXT("temp");
            CHECK(pModule->CallScriptFunc(handle, HE_STR_TEXT("Message"), HE_STR_TEXT("a"), 1, 3.0f,
                                          Core::Common::g_szTempFixedString16));

            // TODO: 引数のあるLua関数を呼び出す事ができるか
            CHECK(pModule->CallScriptFunc(handle, HE_STR_TEXT("MessageArg"), HE_STR_TEXT("a"), 1,
                                          3.0f, Core::Common::g_szTempFixedString16));

            // 同じLuaスクリプトテキストを読み込む場合
            szScriptText = HE_STR_TEXT(
                "function Message2()\n"
                "   HE_LOG(\"田中太郎 ver2\")\n"
                "end\n");

            CHECK(pModule->LoadScriptText(handle, szScriptText));
            // Luaスクリプトの関数を実行
            CHECK(pModule->CallScriptFunc(handle, HE_STR_TEXT("Message2")));
            CHECK(pModule->CallScriptFunc(handle, HE_STR_TEXT("Message")));

            // エラーとなるLuaスクリプトテキストを読み込んだ場合
            szScriptText = HE_STR_TEXT(
                "funcion Message3()\n"
                "   HE_LOG(\"田中太郎 ver3\")\n"
                "end\n");

            CHECK(pModule->LoadScriptText(handle, szScriptText) == FALSE);
            CHECK(pModule->CallScriptFunc(handle, HE_STR_TEXT("Message3")) == FALSE);

            return TRUE;
        });
}

TEST_CASE("Lua Module Script Func Catch")
{
    Uint32 uCount = 0;
    UnitTestRunnerByModuleOnly<Lua::LuaModule>(
        [&uCount]()
        {
            auto pModule = HE_ENGINE.ModuleManager().Get<Lua::LuaModule>();
            if (uCount == 0)
            {
                HE_LOG_LINE(
                    HE_STR_TEXT("LuaスクリプトをロードしてLua関数でコードで登録した関数を呼ぶ"));

                auto f = HE_MAKE_CUSTOM_SHARED_PTR(
                    (Core::Common::FunctionObject<void, Lua::LuaFuncData&>),
                    [](Lua::LuaFuncData& in_rLuaFuncData)
                    {
                        HE_LOG_LINE(in_rLuaFuncData.szFuncName);
                        HE_LOG_LINE(HE_STR_TEXT("%d"), in_rLuaFuncData.uArgCount);

                        if (HE_STR_CMP(in_rLuaFuncData.szFuncName, HE_STR_TEXT("Test_TestFunc")) ==
                            0)
                        {
                            CHECK(in_rLuaFuncData.uArgCount == 1);

                            CHECK(in_rLuaFuncData.aArg[0].eValType == Lua::ELuaFuncArgType_Str);
                            CHECK(HE_STR_CMP(in_rLuaFuncData.aArg[0].data.szText,
                                             HE_STR_TEXT("test")) == 0);
                        }
                        else if (HE_STR_CMP(in_rLuaFuncData.szFuncName,
                                            HE_STR_TEXT("Test_TestFunc02")) == 0)
                        {
                            CHECK(in_rLuaFuncData.uArgCount == 2);

                            CHECK(in_rLuaFuncData.aArg[0].eValType == Lua::ELuaFuncArgType_Float32);
                            CHECK(in_rLuaFuncData.aArg[0].data.fVal == 2.0f);

                            CHECK(in_rLuaFuncData.aArg[1].eValType == Lua::ELuaFuncArgType_Str);
                            CHECK(HE_STR_CMP(in_rLuaFuncData.aArg[1].data.szText,
                                             HE_STR_TEXT("test")) == 0);
                        }
                    });
                pModule->SetEventFunctionByLuaFunc(f);

                auto handle = pModule->CreateLuaObject(HE_STR_TEXT("Test"));
                // Luaスクリプトテキストを読み込む
                Char* szScriptText = HE_STR_TEXT(
                    "function Message()\n"
                    "   Test_TestFunc(\"test\")\n"
                    "end\n"
                    "function Message02()\n"
                    "   Test_TestFunc02(2, \"test\")\n"
                    "end");

                CHECK(pModule->LoadScriptText(handle, szScriptText));

                CHECK(pModule->RegistScriptFunc(handle, HE_STR_TEXT("TestFunc")));

                // TODO: 関数登録とそれを実行するラムダ式でバインドしたい
                /*
                CHECK(pModule->RegistScriptFunc(handle, HE_STR_TEXT("TestFunc")),
                        // 戻り値を使ってコルーチン対応をしたい
                      Core::Common::FunctionObject<void>(
                          [](Char* in_szName) {

                          }));
                */

                CHECK(pModule->RegistScriptFunc(handle, HE_STR_TEXT("TestFunc")));
                CHECK(pModule->RegistScriptFunc(handle, HE_STR_TEXT("TestFunc02")));

                /*
                                CHECK(pModule->RegistScriptFunc(handle,
                   HE_STR_TEXT("TestFunc02")), Core::Common::FunctionObject<void>([](Uint32 in_num,
                   Char* in_szName) {}));
                */
                CHECK(pModule->CallScriptFunc(handle, HE_STR_TEXT("Message")));
                CHECK(pModule->CallScriptFunc(handle, HE_STR_TEXT("Message02")));
            }

            // TODO: Luaスクリプトで登録した関数の結果を出すためにすぐにテストをさせない
            ++uCount;
            return (1 < uCount);
        });
}
