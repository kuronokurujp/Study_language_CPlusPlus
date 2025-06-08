#pragma once

#include "./AssetManagerModule.h"
#include "./PlatformSDL2Module.h"
#include "Engine/Engine.h"

#if 0
// Tomlファイルテスト
TEST_CASE("AssetManager Module TomlFile")
{
    StartupEngineByUnitTest<AssetManager::AssetManagerModule, PlatformSDL2::PlatformSDL2Module>(
        []()
        {
            auto pModule = HE_ENGINE.ModuleManager().Get<AssetManager::AssetManagerModule>();
            pModule->SetCurrentDir(HE_STR_TEXT("Assets"));

            auto handle = pModule->Load<AssetManager::AssetDataToml>(HE_STR_TEXT("test"),
                                                                     HE_STR_TEXT("File/Test.toml"));
            CHECK(handle.Null() == FALSE);
            AssetManager::AssetDataToml& rAsset =
                pModule->GetAsset<AssetManager::AssetDataToml>(handle);

            // 複数名でのノード探索
            {
                auto l = rAsset.VGetNodeByName({HE_STR_U8_TEXT("locate"), HE_STR_U8_TEXT("JP"),
                                                HE_STR_U8_TEXT("default"), HE_STR_U8_TEXT("json")});
                l->VOutputString(&Core::Common::g_szTempFixedString1024);
                CHECK(Core::Common::g_szTempFixedString1024 == HE_STR_TEXT("Text.json"));
            }

            // 取得したノードの子ノード探索
            {
                auto spLayerNode = rAsset.VGetNodeByName({HE_STR_U8_TEXT("locate")});
                spLayerNode->VOutputString(&Core::Common::g_szTempFixedString1024);
                CHECK(Core::Common::g_szTempFixedString1024.Size() <= 0);

                auto spLayerNode = rAsset.VGetNodeByName(*spLayerNode, {HE_STR_U8_TEXT("JP")});
                auto spLabelNode =
                    rAsset.VGetNodeByName(*spLayerNode, {HE_STR_U8_TEXT("debug"), HE_STR_U8_TEXT("json")});

                spLabelNode->VOutputString(&Core::Common::g_szTempFixedString1024);
                CHECK(Core::Common::g_szTempFixedString1024 == HE_STR_TEXT("Debug.json"));
            }

            // レベル指定の子ノード取得
            {
                auto spLayerNode = rAsset.VGetNodeByLevel({HE_STR_U8_TEXT("locate")}, 0);
                CHECK(spLayerNode->IsNone() == FALSE);
                auto spLabelNode = rAsset.VGetNodeByName(*spLayerNode, {HE_STR_U8_TEXT("default"),
                                                                    HE_STR_U8_TEXT("json")});
                CHECK(spLabelNode->IsNone() == FALSE);

                spLabelNode->VOutputString(&Core::Common::g_szTempFixedString1024);
                CHECK(Core::Common::g_szTempFixedString1024 == HE_STR_TEXT("Text.json"));
            }

            return TRUE;
        });
}
#endif

#if 0
// Jsonファイルテスト
TEST_CASE("AssetManager Module JsonFile")
{
    StartupEngineByUnitTest<AssetManager::AssetManagerModule, PlatformSDL2::PlatformSDL2Module>(
        []()
        {
            auto pModule = HE_ENGINE.ModuleManager().Get<AssetManager::AssetManagerModule>();
            pModule->SetCurrentDir(HE_STR_TEXT("Assets"));

            // jsonファイルをロード
            auto handle =
                pModule->Load<AssetManager::AssetDataJson>(HE_STR_U8_TEXT("test"),
                                                           HE_STR_U8_TEXT("File/Test.json"));
            CHECK(handle.Null() == FALSE);
            auto& rAsset = pModule->GetAsset<AssetManager::AssetDataJson>(handle);

            // 一つの名前からのノード探索
            {
                auto spVersionNode = rAsset.VGetNodeByName({HE_STR_U8_TEXT("version")});
                CHECK(spVersionNode->VGetUInt32() == 1);
            }

            // 間違えた探索
            {
                auto spMissNode = rAsset.VGetNodeByName({HE_STR_U8_TEXT("ve")});
                CHECK(spMissNode->IsNone());
            }

            // 複数名を指定したノード探索
            {
                auto spPlayerHPNode =
                    rAsset.VGetNodeByName({HE_STR_U8_TEXT("data"), HE_STR_U8_TEXT("player_default"),
                                           HE_STR_U8_TEXT("hp")});
                CHECK(spPlayerHPNode->VGetUInt32() == 3);
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
                        CHECK(spDefaultNode->VGetUInt32() == 3);
                    }
                    else if (spDefaultNode->GetLevel() == 1)
                    {
                        CHECK(spDefaultNode->VGetFloat32() == 7.0);
                    }
                    else if (spDefaultNode->GetLevel() == 2)
                    {
                        CHECK(spDefaultNode->VGetFloat32() == 1.5);
                    }

                    sLevel = spDefaultNode->GetLevel() + 1;
                }
                CHECK(sLevel == 3);
            }

            // 間違えたノードを指定した場合
            {
                auto spDefaultNode = rAsset.VGetNodeByLevel({HE_STR_U8_TEXT("da")}, 0);
                CHECK(spDefaultNode->IsNone());
            }

            return TRUE;
        });
}
#endif

// xmlファイルテスト
#if 0
TEST_CASE("AssetManager Module TomlFile")
{
    StartupEngineByUnitTest<AssetManager::AssetManagerModule, PlatformSDL2::PlatformSDL2Module>(
        []()
        {
            auto pModule = HE_ENGINE.ModuleManager().Get<AssetManager::AssetManagerModule>();
            pModule->SetCurrentDir(HE_STR_TEXT("Assets"));

            auto handle = pModule->Load<AssetManager::AssetDataXml>(HE_STR_TEXT("test"),
                                                                    HE_STR_TEXT("File/Test.xml"));
            CHECK(handle.Null() == FALSE);
            AssetManager::AssetDataXml& rAsset =
                pModule->GetAsset<AssetManager::AssetDataXml>(handle);

            // 単名でのノード探索
            {
                auto spRootNode = rAsset.VGetNodeByName({HE_STR_U8_TEXT("ui")});
                CHECK(spRootNode->IsNone() == FALSE);

                spRootNode->VOutputString(&Core::Common::g_szTempFixedString1024,
                                          HE_STR_U8_TEXT("id"));
                CHECK(Core::Common::g_szTempFixedString1024 == HE_STR_TEXT("root"));
            }

            // 複数名でのノード探索
            {
                auto spLayerNode =
                    rAsset.VGetNodeByName({HE_STR_U8_TEXT("ui"), HE_STR_U8_TEXT("layer")});
                CHECK(spLayerNode->IsNone() == FALSE);
                CHECK(spLayerNode->VGetUInt32(HE_STR_U8_TEXT("x")) == 0);
                CHECK(spLayerNode->VGetUInt32(HE_STR_U8_TEXT("y")) == 0);
            }

            // 取得したノードの子ノード探索
            {
                auto spRootNode = rAsset.VGetNodeByName({HE_STR_U8_TEXT("ui")});
                CHECK(spRootNode->IsNone() == FALSE);

                auto spLayerNode = rAsset.VGetNodeByName(*spRootNode, {HE_STR_U8_TEXT("layer")});
                CHECK(spLayerNode->IsNone() == FALSE);
                auto spLabelNode = rAsset.VGetNodeByName(*spLayerNode, {HE_STR_U8_TEXT("btn"),
                                                                        HE_STR_U8_TEXT("label")});

                spLabelNode->VOutputString(&Core::Common::g_szTempFixedString1024,
                                           HE_STR_U8_TEXT("text"));
                CHECK(Core::Common::g_szTempFixedString1024 == HE_STR_TEXT("debug.TitleLevelName"));
            }

            // レベル指定の子ノード取得
            {
                auto spLayerNode = rAsset.VGetNodeByLevel({HE_STR_U8_TEXT("ui")}, 0);
                CHECK(spLayerNode->IsNone() == FALSE);
                auto spLabelNode = rAsset.VGetNodeByName(*spLayerNode, {HE_STR_U8_TEXT("btn"),
                                                                        HE_STR_U8_TEXT("label")});
                CHECK(spLabelNode->IsNone() == FALSE);

                spLabelNode->VOutputString(&Core::Common::g_szTempFixedString1024,
                                           HE_STR_U8_TEXT("text"));
                CHECK(Core::Common::g_szTempFixedString1024 == HE_STR_TEXT("debug.TitleLevelName"));
            }

            return TRUE;
        });
}
#endif
