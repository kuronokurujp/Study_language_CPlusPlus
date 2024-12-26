#pragma once

#include "./AssetManagerModule.h"
#include "./LocalizationModule.h"
#include "./PlatformSDL2Module.h"
#include "Engine/Engine.h"

// TODO: テキスト取得
TEST_CASE("Localization Module Text")
{
    UnitTestRunnerByModuleOnly<AssetManager::AssetManagerModule, PlatformSDL2::PlatformSDL2Module,
                               Localization::LocalizationModule>(
        []()
        {
            auto pAssetManagerModule =
                HE_ENGINE.ModuleManager().Get<AssetManager::AssetManagerModule>();
            pAssetManagerModule->SetCurrentDir(HE_STR_TEXT("Assets"));

            auto pLocateModule = HE_ENGINE.ModuleManager().Get<Localization::LocalizationModule>();
            pLocateModule->LoadSystemFile(
                Core::Common::FixedString256(HE_STR_TEXT("Locate/System.toml")));
            pLocateModule->LoadTextAll(Core::Common::FixedString16(HE_STR_TEXT("JP")));

            Core::Common::g_szTempFixedString1024 =
                pLocateModule->Text(HE_STR_TEXT("JP"), HE_STR_TEXT("debug"),
                                    HE_STR_TEXT("TitleLevelName"));

            CHECK(Core::Common::g_szTempFixedString1024 == HE_STR_TEXT("タイトル"));
            HE_LOG_LINE(HE_STR_TEXT("%s"), HE_STR_TEXT("日本語で文字化けしないよ"));

            return TRUE;
        });
}
