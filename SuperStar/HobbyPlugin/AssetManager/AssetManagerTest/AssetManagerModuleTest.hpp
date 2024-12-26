#pragma once

#include "./AssetManagerModule.h"
#include "./PlatformSDL2Module.h"
#include "Engine/Engine.h"

// TODO: Tomlファイルテスト
TEST_CASE("AssetManager Module TomlFile")
{
    UnitTestRunnerByModuleOnly<AssetManager::AssetManagerModule, PlatformSDL2::PlatformSDL2Module>(
        []()
        {
            auto pModule = HE_ENGINE.ModuleManager().Get<AssetManager::AssetManagerModule>();
            pModule->SetCurrentDir(HE_STR_TEXT("Assets"));

            auto handle = pModule->Load<AssetManager::AssetDataToml>(HE_STR_TEXT("test"),
                                                                     HE_STR_TEXT("File/Test.toml"));
            CHECK(handle.Null() == FALSE);
            AssetManager::AssetDataToml& rAsset =
                pModule->GetAsset<AssetManager::AssetDataToml>(handle);

            auto node = rAsset.GetRootNode();
            auto l = node.GetNode(HE_STR_TEXT("locate"), HE_STR_TEXT("JP"), HE_STR_TEXT("default"),
                                  HE_STR_TEXT("json"));
            l.OutputString(&Core::Common::g_szTempFixedString1024);
            CHECK(Core::Common::g_szTempFixedString1024 == HE_STR_TEXT("Text.json"));

            auto l2 = node.GetNode(HE_STR_TEXT("locate"));
            l2.OutputString(&Core::Common::g_szTempFixedString1024);
            CHECK(Core::Common::g_szTempFixedString1024.Size() <= 0);

            auto jp = l2.GetNode(HE_STR_TEXT("JP"));
            jp.GetNode(HE_STR_TEXT("debug"))
                .GetNode(HE_STR_TEXT("json"))
                .OutputString(&Core::Common::g_szTempFixedString1024);
            CHECK(Core::Common::g_szTempFixedString1024 == HE_STR_TEXT("Debug.json"));

            return TRUE;
        });
}
