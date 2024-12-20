#pragma once

#define HE_UNIT_TEST_MODE
#include "../AssetManager/AssetManagerModule.h"
#include "../PlatformSDL2/PlatformSDL2Module.h"
#include "../Render/RenderModule.h"
#include "Engine/Common/Function.h"
#include "Engine/Engine.h"

// GUIテスト
TEST_CASE("SDL2 Font Load GUITest")
{
    HE::Uint32 uStep = 0;
    Core::Common::Handle sceneHandle;
    Core::Common::Handle fontMatHandle;

    UnitTestRunnerByModuleOnly<AssetManager::AssetManagerModule, PlatformSDL2::PlatformSDL2Module,
                               Render::RenderModule>(
        [&uStep, &sceneHandle, &fontMatHandle]()
        {
            auto pPlatformModule = HE_ENGINE.PlatformModule();
            auto pRenderModule   = HE_ENGINE.ModuleManager().Get<Render::RenderModule>();

            if (uStep == 1)
            {
                auto pAssetManagerModule =
                    HE_ENGINE.ModuleManager().Get<AssetManager::AssetManagerModule>();
                pAssetManagerModule->SetCurrentDir(HE_STR_TEXT("Assets"));

                // フォントデータのバイナリアセットを作成
                {
                    // TODO: ロードするフォントファイルパスを渡す
                    // シェーダーファイルも渡す.(プラットフォームによっては使わない)
                    auto bRet = pPlatformModule->VFont()->VLoad(32, {"Font/TestFont.ttf",
                                                                     "Shader/Font.vert",
                                                                     "Shader/Font.frag"});
                    CHECK(bRet);
                }

                // TODO: プラットフォームを他のモジュールから絶対参照しないようにする
                // 他のモジュールの事情に合わせる作りすると拡張と修正が困難だから
                // プラットフォームはロウモジュールでロウモジュールは他のモジュールの参照をしてはいけない
                // 依存していたらエラーにする仕組みを用意するべきかも

                ++uStep;
                // TODO: ここまで動くか
                return FALSE;
            }
            else if (uStep == 0)
            {
                Core::Common::Handle windowHandle;
                Core::Common::Handle viewPortHandle;
                {
                    // TODO: 外部設定が必要かも
                    // ゲームウィンドウを生成
                    windowHandle = pRenderModule->NewWindow(640, 480);

                    // TODO: 画面に表示するビューポート
                    // ゲームウィンドウで利用するビューポートを追加
                    viewPortHandle = pRenderModule->AddViewPort(windowHandle, 640, 480);
                }

                // UIのゲームシーンを追加
                {
                    auto [handle, pScene] =
                        pRenderModule->AddSceneViewUI(windowHandle, viewPortHandle);
                    sceneHandle = handle;
                }

                // ゲームウィンドウを表示
                pRenderModule->ShowWindow(windowHandle);

                ++uStep;

                return FALSE;
            }
            else if (uStep == 2)
            {
                // TODO: ウィンドウが閉じたら終了
                // TODO: 作成したリソース解放される
                Core::Common::FixedString1024 s(HE_STR_TEXT("T"));
                Render::Command2DTextDraw(sceneHandle, Core::Math::Vector2(0.0f, 0.0f), s,
                                          Core::Math::RGB::White,
                                          Core::Math::Rect2::EAnchor::EAnchor_Center);

                return FALSE;
            }

            return TRUE;
        });
}

TEST_CASE("SDL2 Load ImgTexture")
{
    UnitTestRunnerByModuleOnly<AssetManager::AssetManagerModule, PlatformSDL2::PlatformSDL2Module,
                               Render::RenderModule>(
        []()
        {
            /*
                auto pPlatformModule = HE_ENGINE.PlatformModule();
                // ゲームウィンドウを作成
                auto spScreen = pPlatformModule->VScreen();
                HE_ASSERT(spScreen);

                auto pAssetManagerModule =
                    HE_ENGINE.ModuleManager().Get<AssetManager::AssetManagerModule>();
                pAssetManagerModule->SetMountDir(HE_STR_TEXT("Assets"));

                // 画像データのバイナリアセットを作成
                auto handle =
                    pAssetManagerModule->Load<AssetManager::AssetDataBinary>("Image",
                                                                             "Img/TestImg.png");
                auto& rAsset = pAssetManagerModule->GetAsset<AssetManager::AssetDataBinary>(handle);


                auto imgHandle = spScreen->VCreateTextureImage(rAsset.Mem(), rAsset.Size());
                CHECK(imgHandle.Null() == FALSE);

                pAssetManagerModule->Unload(handle);

                CHECK(spScreen->VReleaseTexture(imgHandle));
                */

            return TRUE;
        });
}
