#include <gtest/gtest.h>

#include "AssetManagerModule.h"
#include "Engine/Engine.h"
#include "PlatformSDL2Module.h"

namespace AssetManager
{
    // Tomlファイルテスト
    TEST(AssetManager, TomlFile)
    {
        // TODO: 使うモジュールを作成
        auto spSDL2Module         = HE_MAKE_CUSTOM_SHARED_PTR((PlatformSDL2::PlatformSDL2Module));
        auto spAssetManagerModule = HE_MAKE_CUSTOM_SHARED_PTR((AssetManager::AssetManagerModule));

        Module::ModuleManager moduleManager;
        EXPECT_EQ(moduleManager.AddModule(spSDL2Module), TRUE);
        EXPECT_EQ(moduleManager.AddModule(spAssetManagerModule), TRUE);

        EXPECT_EQ(moduleManager.Start(), TRUE);

        // TODO: アセットをロードするディレクトリを指定
        spAssetManagerModule->SetCurrentDir(
            Core::File::Path(HE_STR_TEXT("Resources"), HE_STR_TEXT("AssetManager")).Str());

        // TODO: アセットの同期ロード
        auto handle = spAssetManagerModule->Load<AssetManager::AssetDataToml>(
            HE_STR_TEXT("test"), Core::File::Path(HE_STR_TEXT("File"), HE_STR_TEXT("Test.toml")));
        EXPECT_EQ(handle.Null(), FALSE);

        AssetManager::AssetDataToml& rAsset =
            spAssetManagerModule->GetAsset<AssetManager::AssetDataToml>(handle);

        // 複数名でのノード探索
        {
            auto l = rAsset.VGetNodeByName({HE_STR_U8_TEXT("locate"), HE_STR_U8_TEXT("JP"),
                                            HE_STR_U8_TEXT("default"), HE_STR_U8_TEXT("json")});
            l->VOutputString(&Core::Common::g_szTempFixedString1024);
            EXPECT_EQ(Core::Common::g_szTempFixedString1024, HE_STR_TEXT("Text.json"));
        }

        // 取得したノードの子ノード探索
        {
            auto spLayerNode = rAsset.VGetNodeByName({HE_STR_U8_TEXT("locate")});
            spLayerNode->VOutputString(&Core::Common::g_szTempFixedString1024);
            EXPECT_LE(Core::Common::g_szTempFixedString1024.Size(), 0);

            spLayerNode      = rAsset.VGetNodeByName(*spLayerNode, {HE_STR_U8_TEXT("JP")});
            auto spLabelNode = rAsset.VGetNodeByName(*spLayerNode, {HE_STR_U8_TEXT("debug"),
                                                                    HE_STR_U8_TEXT("json")});

            spLabelNode->VOutputString(&Core::Common::g_szTempFixedString1024);
            EXPECT_EQ(Core::Common::g_szTempFixedString1024, HE_STR_TEXT("Debug.json"));
        }

        // レベル指定の子ノード取得
        {
            auto spLayerNode = rAsset.VGetNodeByLevel({HE_STR_U8_TEXT("locate")}, 0);
            EXPECT_EQ(spLayerNode->IsNone(), FALSE);
            auto spLabelNode = rAsset.VGetNodeByName(*spLayerNode, {HE_STR_U8_TEXT("default"),
                                                                    HE_STR_U8_TEXT("json")});
            EXPECT_EQ(spLabelNode->IsNone(), FALSE);

            spLabelNode->VOutputString(&Core::Common::g_szTempFixedString1024);
            EXPECT_EQ(Core::Common::g_szTempFixedString1024, HE_STR_TEXT("Text.json"));
        }

        EXPECT_EQ(moduleManager.Release(), TRUE);
    }

    // Jsonファイルテスト
    TEST(AssetManager, JsonFile)
    {
        // TODO: 使うモジュールを作成
        auto spSDL2Module         = HE_MAKE_CUSTOM_SHARED_PTR((PlatformSDL2::PlatformSDL2Module));
        auto spAssetManagerModule = HE_MAKE_CUSTOM_SHARED_PTR((AssetManager::AssetManagerModule));

        Module::ModuleManager moduleManager;
        EXPECT_EQ(moduleManager.AddModule(spSDL2Module), TRUE);
        EXPECT_EQ(moduleManager.AddModule(spAssetManagerModule), TRUE);

        EXPECT_EQ(moduleManager.Start(), TRUE);

        // TODO: アセットをロードするディレクトリを指定
        spAssetManagerModule->SetCurrentDir(
            Core::File::Path(HE_STR_TEXT("Resources"), HE_STR_TEXT("AssetManager")).Str());

        // jsonファイルをロード
        auto handle =
            spAssetManagerModule->Load<AssetManager::AssetDataJson>(HE_STR_U8_TEXT("test"),
                                                                    HE_STR_U8_TEXT(
                                                                        "File/Test.json"));
        EXPECT_EQ(handle.Null(), FALSE);
        auto& rAsset = spAssetManagerModule->GetAsset<AssetManager::AssetDataJson>(handle);

        // 一つの名前からのノード探索
        {
            auto spVersionNode = rAsset.VGetNodeByName({HE_STR_U8_TEXT("version")});
            EXPECT_EQ(spVersionNode->VGetUInt32(), 1);
        }

        // 間違えた探索
        {
            auto spMissNode = rAsset.VGetNodeByName({HE_STR_U8_TEXT("ve")});
            EXPECT_EQ(spMissNode->IsNone(), TRUE);
        }

        // 複数名を指定したノード探索
        {
            auto spPlayerHPNode = rAsset.VGetNodeByName(
                {HE_STR_U8_TEXT("data"), HE_STR_U8_TEXT("player_default"), HE_STR_U8_TEXT("hp")});
            EXPECT_EQ(spPlayerHPNode->VGetUInt32(), 3);
        }

        // レベルを使ったノード探索
        {
            HE::Sint32 sLevel = 0;
            while (TRUE)
            {
                auto spDefaultNode = rAsset.VGetNodeByLevel({HE_STR_U8_TEXT("data"),
                                                             HE_STR_U8_TEXT("player_default")},
                                                            sLevel);
                if (spDefaultNode->IsNone()) break;
                if (spDefaultNode->GetLevel() == 0)
                {
                    EXPECT_EQ(spDefaultNode->VGetUInt32(), 3);
                }
                else if (spDefaultNode->GetLevel() == 1)
                {
                    EXPECT_EQ(spDefaultNode->VGetFloat32(), 7.0);
                }
                else if (spDefaultNode->GetLevel() == 2)
                {
                    EXPECT_EQ(spDefaultNode->VGetFloat32(), 1.5);
                }

                sLevel = spDefaultNode->GetLevel() + 1;
            }
            EXPECT_EQ(sLevel, 3);
        }

        // 間違えたノードを指定した場合
        {
            auto spDefaultNode = rAsset.VGetNodeByLevel({HE_STR_U8_TEXT("da")}, 0);
            EXPECT_EQ(spDefaultNode->IsNone(), TRUE);
        }

        EXPECT_EQ(moduleManager.Release(), TRUE);
    }

    // xmlファイルテスト
    TEST(AssetManager, XmlFile)
    {
        // TODO: 使うモジュールを作成
        auto spSDL2Module         = HE_MAKE_CUSTOM_SHARED_PTR((PlatformSDL2::PlatformSDL2Module));
        auto spAssetManagerModule = HE_MAKE_CUSTOM_SHARED_PTR((AssetManager::AssetManagerModule));

        Module::ModuleManager moduleManager;
        EXPECT_EQ(moduleManager.AddModule(spSDL2Module), TRUE);
        EXPECT_EQ(moduleManager.AddModule(spAssetManagerModule), TRUE);

        EXPECT_EQ(moduleManager.Start(), TRUE);

        // TODO: アセットをロードするディレクトリを指定
        spAssetManagerModule->SetCurrentDir(
            Core::File::Path(HE_STR_TEXT("Resources"), HE_STR_TEXT("AssetManager")).Str());

        auto handle =
            spAssetManagerModule->Load<AssetManager::AssetDataXml>(HE_STR_TEXT("test"),
                                                                   HE_STR_TEXT("File/Test.xml"));
        EXPECT_EQ(handle.Null(), FALSE);
        AssetManager::AssetDataXml& rAsset =
            spAssetManagerModule->GetAsset<AssetManager::AssetDataXml>(handle);

        // 単名でのノード探索
        {
            auto spRootNode = rAsset.VGetNodeByName({HE_STR_U8_TEXT("ui")});
            EXPECT_EQ(spRootNode->IsNone(), FALSE);

            spRootNode->VOutputString(&Core::Common::g_szTempFixedString1024, HE_STR_U8_TEXT("id"));
            EXPECT_EQ(Core::Common::g_szTempFixedString1024, HE_STR_TEXT("root"));
        }

        // 複数名でのノード探索
        {
            auto spLayerNode =
                rAsset.VGetNodeByName({HE_STR_U8_TEXT("ui"), HE_STR_U8_TEXT("layer")});
            EXPECT_EQ(spLayerNode->IsNone(), FALSE);
            EXPECT_EQ(spLayerNode->VGetUInt32(HE_STR_U8_TEXT("x")), 0);
            EXPECT_EQ(spLayerNode->VGetUInt32(HE_STR_U8_TEXT("y")), 0);
        }

        // 取得したノードの子ノード探索
        {
            auto spRootNode = rAsset.VGetNodeByName({HE_STR_U8_TEXT("ui")});
            EXPECT_EQ(spRootNode->IsNone(), FALSE);

            auto spLayerNode = rAsset.VGetNodeByName(*spRootNode, {HE_STR_U8_TEXT("layer")});
            EXPECT_EQ(spLayerNode->IsNone(), FALSE);
            auto spLabelNode = rAsset.VGetNodeByName(*spLayerNode, {HE_STR_U8_TEXT("btn"),
                                                                    HE_STR_U8_TEXT("label")});

            spLabelNode->VOutputString(&Core::Common::g_szTempFixedString1024,
                                       HE_STR_U8_TEXT("text"));
            EXPECT_EQ(Core::Common::g_szTempFixedString1024, HE_STR_TEXT("debug.TitleLevelName"));
        }

        // レベル指定の子ノード取得
        {
            auto spLayerNode = rAsset.VGetNodeByLevel({HE_STR_U8_TEXT("ui")}, 0);
            EXPECT_EQ(spLayerNode->IsNone(), FALSE);
            auto spLabelNode = rAsset.VGetNodeByName(*spLayerNode, {HE_STR_U8_TEXT("btn"),
                                                                    HE_STR_U8_TEXT("label")});
            EXPECT_EQ(spLabelNode->IsNone(), FALSE);

            spLabelNode->VOutputString(&Core::Common::g_szTempFixedString1024,
                                       HE_STR_U8_TEXT("text"));
            EXPECT_EQ(Core::Common::g_szTempFixedString1024, HE_STR_TEXT("debug.TitleLevelName"));
        }

        EXPECT_EQ(moduleManager.Release(), TRUE);
    }
}  // namespace AssetManager
