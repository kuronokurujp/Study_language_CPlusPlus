#include <gtest/gtest.h>

#include "AssetManagerModule.h"
#include "LocalizationModule.h"
#include "PlatformSDL2Module.h"

namespace Localization
{
    TEST(HobbyPlugin_Localization, Text)
    {
        auto spLocalizationModule = HE_MAKE_CUSTOM_SHARED_PTR((Localization::LocalizationModule));
        // ローカライズモジュールを動かすのに必要なモジュールを作る
        auto spAssetManagerModule = HE_MAKE_CUSTOM_SHARED_PTR((AssetManager::AssetManagerModule));
        auto spSDL2Module         = HE_MAKE_CUSTOM_SHARED_PTR((PlatformSDL2::PlatformSDL2Module));

        Module::ModuleManager moduleManager;
        EXPECT_EQ(moduleManager.AddModule(spLocalizationModule), TRUE);
        EXPECT_EQ(moduleManager.AddModule(spAssetManagerModule), TRUE);
        EXPECT_EQ(moduleManager.AddModule(spSDL2Module), TRUE);

        moduleManager.Start();

        // アセットデータを扱うフォルダを指定
        spAssetManagerModule->SetCurrentDir(HE_STR_TEXT("Resources/Localization"));

        spLocalizationModule->LoadSystemFile(
            Core::Common::FixedString256(HE_STR_TEXT("System.toml")));
        spLocalizationModule->LoadTextAll(Core::Common::FixedString16(HE_STR_TEXT("JP")));

        Core::Common::g_szTempFixedString1024 =
            spLocalizationModule->Text(HE_STR_TEXT("JP"), HE_STR_TEXT("debug"),
                                       HE_STR_TEXT("TitleLevelName"));

        EXPECT_TRUE(Core::Common::g_szTempFixedString1024 == HE_STR_TEXT("タイトル"));

        EXPECT_TRUE(moduleManager.Release());
    }
}  // namespace Localization
