#pragma once

// テストをする
#include "../LuaModule.h"
#include "Engine/Common/Function.h"
#include "Engine/Engine.h"

TEST_CASE("Lua Module Create And Release")
{
    StartupEngineByUnitTest<Lua::LuaModule>(
        []()
        {
            auto pModule = HE_ENGINE.ModuleManager().Get<Lua::LuaModule>();

            HE_LOG_LINE(HE_STR_TEXT("Luaモジュールテストできるー"));
            auto handle   = pModule->CreateLuaObject(HE_STR_TEXT("Test"));
            auto handle01 = pModule->CreateLuaObject(HE_STR_TEXT("Test2"));
            pModule->ReleaseLuaObject(handle);
            pModule->ReleaseLuaObject(handle01);

            return TRUE;
        });
}

TEST_CASE("Lua Module Test Call Func")
{
    StartupEngineByUnitTest<Lua::LuaModule>(
        []()
        {
            auto pModule = HE_ENGINE.ModuleManager().Get<Lua::LuaModule>();

            HE_LOG_LINE(HE_STR_TEXT("LuaスクリプトをロードしてLua関数を実行"));

            auto handle = pModule->CreateLuaObject(HE_STR_TEXT("Test"));
            // Luaスクリプトテキストを読み込む
            HE::Char* szScriptText = HE_STR_TEXT(
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
            // functionがfuncionになっている
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
    HE::Uint32 uCount = 0;
    StartupEngineByUnitTest<Lua::LuaModule>(
        [&uCount]()
        {
            auto pModule = HE_ENGINE.ModuleManager().Get<Lua::LuaModule>();
            if (uCount == 0)
            {
                HE_LOG_LINE(
                    HE_STR_TEXT("LuaスクリプトをロードしてLua関数でコードで登録した関数を呼ぶ"));

                auto f = HE_MAKE_CUSTOM_UNIQUE_PTR(
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
                            CHECK(HE_STR_CMP(in_rLuaFuncData.aArg[0]._data.szText,
                                             HE_STR_TEXT("test")) == 0);
                        }
                        else if (HE_STR_CMP(in_rLuaFuncData.szFuncName,
                                            HE_STR_TEXT("Test_TestFunc02")) == 0)
                        {
                            CHECK(in_rLuaFuncData.uArgCount == 3);

                            CHECK(in_rLuaFuncData.aArg[0].eValType == Lua::ELuaFuncArgType_Float32);
                            CHECK(in_rLuaFuncData.aArg[0]._data.fVal == 2.0f);

                            CHECK(in_rLuaFuncData.aArg[1].eValType == Lua::ELuaFuncArgType_Str);
                            CHECK(HE_STR_CMP(in_rLuaFuncData.aArg[1]._data.szText,
                                             HE_STR_TEXT("test")) == 0);

                            CHECK(in_rLuaFuncData.aArg[2].eValType == Lua::ELuaFuncArgType_Uint64);
                            CHECK(in_rLuaFuncData.aArg[2]._data.uuVal == 3);
                        }
                    });
                // Luaスクリプトから呼び出した関数を受け取れるようにする
                pModule->AddListenerLuaFunc(std::move(f));

                // Luaスクリプトが呼び出す関数の接頭語Testを設定
                auto handle = pModule->CreateLuaObject(HE_STR_TEXT("Test"));
                // Luaスクリプトテキストを読み込む
                HE::Char* szScriptText = HE_STR_TEXT(
                    "function Message()\n"
                    "   Test_TestFunc(\"test\")\n"
                    "end\n"
                    "function Message02()\n"
                    "   Test_TestFunc02(2.0, \"test\", 3)\n"
                    "end");

                CHECK(pModule->LoadScriptText(handle, szScriptText));

                CHECK(pModule->RegistScriptFunc(handle, HE_STR_TEXT("Test_TestFunc")));
                // 呼び出す関数を二重設定しても問題ないか
                CHECK(pModule->RegistScriptFunc(handle, HE_STR_TEXT("Test_TestFunc")));

                CHECK(pModule->RegistScriptFunc(handle, HE_STR_TEXT("Test_TestFunc02")));

                CHECK(pModule->CallScriptFunc(handle, HE_STR_TEXT("Message")));
                CHECK(pModule->CallScriptFunc(handle, HE_STR_TEXT("Message02")));
            }

            // Luaスクリプトで登録した関数の結果を出すためにすぐにテスト終了にさせない
            ++uCount;
            return (1 < uCount);
        });
}
