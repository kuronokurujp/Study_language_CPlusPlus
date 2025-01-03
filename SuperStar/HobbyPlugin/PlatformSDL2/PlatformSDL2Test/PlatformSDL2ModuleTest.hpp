#pragma once

#define HE_UNIT_TEST_MODE
#include "../AssetManager/AssetManagerModule.h"
#include "../PlatformSDL2/PlatformSDL2Module.h"
#include "../Render/RenderModule.h"
#include "Engine/Common/Function.h"
#include "Engine/Engine.h"

// GUIテスト
// ウィンドウが開く
// TODO: ウィンドウで表示後に一定時間後に画像保存する仕組みがほしいな
/*
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

            if (uStep == 0)
            {
                auto pAssetManagerModule =
                    HE_ENGINE.ModuleManager().Get<AssetManager::AssetManagerModule>();
                pAssetManagerModule->SetCurrentDir(HE_STR_TEXT("Assets"));

                // フォントデータのバイナリアセットを作成
                {
                    // ロードするフォントファイルパスを渡す
                    // シェーダーファイルも渡す.(プラットフォームによっては使わない)
                    auto bRet = pPlatformModule->VFont()->VLoad(Platform::EFontSize_64,
                                                                HE_STR_TEXT("Font/TestFont.ttf"));
                    CHECK(bRet);
                }

                ++uStep;
                return FALSE;
            }
            else if (uStep == 1)
            {
                Core::Common::Handle windowHandle;
                Core::Common::Handle viewPortHandle;
                {
                    // ゲームウィンドウを生成
                    windowHandle = pRenderModule->NewWindow(640, 480);

                    // 画面に表示するビューポート
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
                // ウィンドウが閉じたら終了

                // TODO: UIテキストを表示
                // 2改行チェック
                Core::Common::FixedString1024 s(HE_STR_TEXT("カTestあ\n12\n34"));
                Render::Command2DTextDraw(sceneHandle, Core::Math::Vector2(0.0f, 0.0f), s, 32,
                                          Core::Math::RGB::White,
                                          Core::Math::Rect2::EAnchor::EAnchor_Left);

                // 1改行チェック
                Core::Common::FixedString1024 s1(HE_STR_TEXT("T\nT"));
                Render::Command2DTextDraw(sceneHandle, Core::Math::Vector2(0.0f, 32.0f * 3.f), s1,
                                          32, Core::Math::RGB::White,
                                          Core::Math::Rect2::EAnchor::EAnchor_Left);

                //  改行なしチェック
                Core::Common::FixedString1024 s2(HE_STR_TEXT("Ss"));
                Render::Command2DTextDraw(sceneHandle, Core::Math::Vector2(0.0f, 32.0f * 5.f), s2,
                                          32, Core::Math::RGB::White,
                                          Core::Math::Rect2::EAnchor::EAnchor_Left);

                return FALSE;
            }

            return TRUE;
        });
}
*/

/*
TEST_CASE("SDL2 Quad Draw GUITest")
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

            if (uStep == 0)
            {
                ++uStep;
                auto pAssetManagerModule =
                    HE_ENGINE.ModuleManager().Get<AssetManager::AssetManagerModule>();
                pAssetManagerModule->SetCurrentDir(HE_STR_TEXT("Assets"));

                // フォントデータのバイナリアセットを作成
                {
                    // ロードするフォントファイルパスを渡す
                    auto bRet = pPlatformModule->VFont()->VLoad(Platform::EFontSize_64,
                                                                HE_STR_TEXT("Font/TestFont.ttf"));
                    CHECK(bRet);
                }
                return FALSE;
            }
            else if (uStep == 1)
            {
                ++uStep;
                Core::Common::Handle windowHandle;
                Core::Common::Handle viewPortHandle;
                {
                    // ゲームウィンドウを生成
                    windowHandle = pRenderModule->NewWindow(640, 480);

                    // 画面に表示するビューポート
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
            else if (uStep == 3)
            {
                // ウィンドウが閉じたら終了
                Core::Math::Rect2 rect;
                // 左隅にぴったり表示しているかのテスト
                {
                    rect.SetPosition(0.f, 0.0f, 32.0f, 32.0f, Core::Math::EAnchor_Left);
                    Render::Command2DQuadDraw(sceneHandle, rect, Core::Math::RGB::Red);
                }

                // 左隅の矩形と1ドット右にずらして矩形表示しているかテスト
                {
                    rect.SetPosition(33.f, 0.0f, 32.0f, 32.0f, Core::Math::EAnchor_Left);
                    Render::Command2DQuadDraw(sceneHandle, rect, Core::Math::RGB::Yellow);
                }

                // 左隅の矩形と1ドット下にずらして矩形表示しているかテスト
                {
                    rect.SetPosition(0.f, 33.0f, 32.0f, 32.0f, Core::Math::EAnchor_Left);
                    Render::Command2DQuadDraw(sceneHandle, rect, Core::Math::RGB::Yellow);
                }

                // テキストの下地で矩形が表示しているかテスト
                {
                    rect.SetPosition(0.f, 32.0f * 5.0f, 32.0f * 5.f, 32.0f,
                                     Core::Math::EAnchor_Left);
                    Render::Command2DQuadDraw(sceneHandle, rect, Core::Math::RGB::Red);

                    // テキスト表示
                    Core::Common::FixedString1024 s2(
                        HE_STR_TEXT("タイトルルルルルルルルルルルルルルるあ"));
                    Render::Command2DTextDraw(sceneHandle, Core::Math::Vector2(0.0f, 32.0f * 5.f),
                                              s2, 32, Core::Math::RGB::Blue,
                                              Core::Math::EAnchor_Left, 2.0f);

                    Core::Common::FixedString1024 s3(HE_STR_TEXT("MMM"));
                    Render::Command2DTextDraw(sceneHandle, Core::Math::Vector2(0.0f, 64.0f * 5.f),
                                              s3, 16, Core::Math::RGB::Blue,
                                              Core::Math::EAnchor_Left, 2.0f);
                }

                // 画面真ん中に矩形表示のアンカーが真ん中
                {
                    rect.SetPosition(320.0f, 240.0f, 32.0f, 32.0f, Core::Math::EAnchor_Center);
                    Render::Command2DQuadDraw(sceneHandle, rect, Core::Math::RGB::Red);
                }

                // 画面真ん中に矩形表示してかつアンカーが左上
                {
                    rect.SetPosition(320.0f, 240.0f, 32.0f, 32.0f, Core::Math::EAnchor_Left);
                    Render::Command2DQuadDraw(sceneHandle, rect, Core::Math::RGB::White);
                }

                // 画面左下に矩形表示してかつアンカーが左上
                {
                    rect.SetPosition(0.0f, 480.0f - 32.0f, 32.0f, 32.0f, Core::Math::EAnchor_Left);
                    Render::Command2DQuadDraw(sceneHandle, rect, Core::Math::RGB::White);
                }

                // 画面右上に矩形表示してかつアンカーが左上
                {
                    rect.SetPosition(640.0 - 32.0f, 0.0f, 32.0f, 32.0f, Core::Math::EAnchor_Left);
                    Render::Command2DQuadDraw(sceneHandle, rect, Core::Math::RGB::White);
                }

                // 画面右下に矩形表示してかつアンカーが左上
                {
                    rect.SetPosition(640.0f - 32.0f, 480.0f - 32.0f, 32.0f, 32.0f,
                                     Core::Math::EAnchor_Left);
                    Render::Command2DQuadDraw(sceneHandle, rect, Core::Math::RGB::White);
                }

                // アンカーが左上で円描画の画面真ん中上のテスト
                {
                    Core::Math::Vector2 pos(320.0f, 0.0f);
                    Render::Command2DCircleDraw(sceneHandle, pos, Core::Math::EAnchor_Left, 32,
                                                Core::Math::RGB::White);
                }

                // アンカーが中心で画面真ん中下の円描画のテスト
                {
                    Core::Math::Vector2 pos(320.0f, 480.f - 16.0f);
                    Render::Command2DCircleDraw(sceneHandle, pos, Core::Math::EAnchor_Center, 32,
                                                Core::Math::RGB::White);
                }

                // アンカー中心三角形描画テスト
                {
                    Core::Math::Vector2 pos(480.0f, 480.f - 32.0f);
                    Render::Command2DTriangleDraw(sceneHandle, pos, Core::Math::EAnchor_Center,
                                                  -90.0f, 32.0f, Core::Math::RGB::White);
                }

                // アンカー左上の三角形描画テスト
                {
                    Core::Math::Vector2 pos(480.0f, 0.f);
                    Render::Command2DTriangleDraw(sceneHandle, pos, Core::Math::EAnchor_Left,
                                                  -90.0f, 32.0f, Core::Math::RGB::White);
                }

                // 中心位置にテキスト表示
                {
                    Core::Common::FixedString1024 s3(HE_STR_TEXT("AABBBBBBBAABBBBBBBAA"));
                    //Core::Common::FixedString1024 s3(HE_STR_TEXT("ABA"));
                    //Core::Common::FixedString1024 s3(HE_STR_TEXT("AAAAAABBAAAAAA"));
                    //Core::Common::FixedString1024 s3(HE_STR_TEXT("B"));
                    Render::Command2DTextDraw(sceneHandle, Core::Math::Vector2(320.0f, 240.0f),
                                              s3, 32, Core::Math::RGB::Blue,
                                              Core::Math::EAnchor_Center, 0);
                }

                return FALSE;
            }

            return TRUE;
        });
}
*/

/*
TEST_CASE("SDL2 Draw Particle")
{
    HE::Uint32 uStep = 0;
    Core::Common::Handle renderHandle;
    Core::Common::Handle fontMatHandle;

    Core::Common::Handle prticleHandle;

    HE::Uint32 uMoveStep = 0;
    Core::Math::Vector2 pos(320.f, 240.0f);
    UnitTestRunnerByModuleOnly<AssetManager::AssetManagerModule, PlatformSDL2::PlatformSDL2Module,
                               Render::RenderModule>(
        [&uStep, &renderHandle, &fontMatHandle, &uMoveStep, &pos, &prticleHandle]()
        {
            auto pPlatformModule = HE_ENGINE.PlatformModule();
            auto pRenderModule   = HE_ENGINE.ModuleManager().Get<Render::RenderModule>();

            if (uStep == 0)
            {
                ++uStep;
                auto pAssetManagerModule =
                    HE_ENGINE.ModuleManager().Get<AssetManager::AssetManagerModule>();
                pAssetManagerModule->SetCurrentDir(HE_STR_TEXT("Assets"));

                return FALSE;
            }
            else if (uStep == 1)
            {
                ++uStep;
                Core::Common::Handle windowHandle;
                Core::Common::Handle viewPortHandle;
                {
                    // ゲームウィンドウを生成
                    windowHandle = pRenderModule->NewWindow(640, 480);

                    // 画面に表示するビューポート
                    // ゲームウィンドウで利用するビューポートを追加
                    viewPortHandle = pRenderModule->AddViewPort(windowHandle, 640, 480);
                }

                // UIのゲームシーンを追加
                {
                    auto [handle, pScene] =
                        pRenderModule->AddSceneViewUI(windowHandle, viewPortHandle);
                    renderHandle = handle;
                }

                // ゲームウィンドウを表示
                pRenderModule->ShowWindow(windowHandle);

                ++uStep;

                return FALSE;
            }
            else if (uStep == 3)
            {
                // TODO: パーティクル作成
                {
                    Core::Common::FixedArray<Core::Math::Vector3, 1000> aPos;
                    Core::Common::FixedArray<Core::Math::Vector3, 1000> aVelocity;
                    Core::Common::FixedArray<Core::Math::Color, 1000> aColor;

                    for (HE::Uint32 i = 0; i < 1000; ++i)
                    {
                        auto x = pPlatformModule->VSystem()->VGetRandByFloat(-1.0f, 1.0f);
                        auto y = pPlatformModule->VSystem()->VGetRandByFloat(-1.0f, 1.0f);

                        Core::Math::Vector3 p(static_cast<HE::Float32>(x),
                                              static_cast<HE::Float32>(y), 0.0f);
                        aPos.Set(i, p);

                        Core::Math::Vector3 v(0.0f, 0.0f, 0.0f);
                        aVelocity.Set(i, v);

                        aColor.Set(i, Core::Math::RGB::White);
                    }

                    auto [handle, pBlob] = pRenderModule->CreatePrticle(renderHandle);
                    pBlob->Init(aPos.Capacity());
                    prticleHandle = handle;

                    pBlob->SetPositions(aPos);
                    pBlob->SetVelocitys(aVelocity);
                    pBlob->SetColors(aColor);
                }
                ++uStep;

                return FALSE;
            }
            else if (uStep == 4)
            {
                // ウィンドウが閉じたら終了
                // TODO: 点群移動
                // TODO: 点群表示
                Render::Command2DParticalDraw(renderHandle, prticleHandle, pos);
                pos._fY -= 0.5f;

                return FALSE;
            }

            return TRUE;
        },
        [&prticleHandle]()
        {
            auto pPlatformModule = HE_ENGINE.PlatformModule();
            auto pRenderModule   = HE_ENGINE.ModuleManager().Get<Render::RenderModule>();
            pRenderModule->DeletePrticle(prticleHandle);
        });
}
*/

TEST_CASE("SDL2 Draw MoveTest")
{
    HE::Uint32 uStep = 0;
    Core::Common::Handle sceneHandle;
    Core::Common::Handle fontMatHandle;

    HE::Uint32 uMoveStep = 0;
    Core::Math::Vector2 pos(100, 100);
    UnitTestRunnerByModuleOnly<AssetManager::AssetManagerModule, PlatformSDL2::PlatformSDL2Module,
                               Render::RenderModule>(
        [&uStep, &sceneHandle, &fontMatHandle, &uMoveStep, &pos]()
        {
            auto pPlatformModule = HE_ENGINE.PlatformModule();
            auto pRenderModule   = HE_ENGINE.ModuleManager().Get<Render::RenderModule>();

            if (uStep == 0)
            {
                ++uStep;
                auto pAssetManagerModule =
                    HE_ENGINE.ModuleManager().Get<AssetManager::AssetManagerModule>();
                pAssetManagerModule->SetCurrentDir(HE_STR_TEXT("Assets"));

                // フォントデータのバイナリアセットを作成
                {
                    // ロードするフォントファイルパスを渡す
                    auto bRet = pPlatformModule->VFont()->VLoad(Platform::EFontSize_64,
                                                                HE_STR_TEXT("Font/TestFont.ttf"));
                    CHECK(bRet);
                }
                return FALSE;
            }
            else if (uStep == 1)
            {
                ++uStep;
                Core::Common::Handle windowHandle;
                Core::Common::Handle viewPortHandle;
                {
                    // ゲームウィンドウを生成
                    windowHandle = pRenderModule->NewWindow(640, 480);

                    // 画面に表示するビューポート
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
            else if (uStep == 3)
            {
                // ウィンドウが閉じたら終了
                // 左隅にぴったり表示しているかのテスト
                Core::Math::Rect2 rect;
                rect.SetPosition(pos._fX, pos._fY, 32.0f, 32.0f, Core::Math::EAnchor_Left);
                auto fM = 1.0f;
                if (uMoveStep == 0)
                {
                    pos._fX += fM;
                    if (640.0f <= pos._fX)
                    {
                        uMoveStep = 1;
                    }
                }
                else
                {
                    pos._fX -= fM;
                    if (pos._fX <= 0.0f)
                    {
                        uMoveStep = 0;
                    }
                }

                Render::Command2DQuadDraw(sceneHandle, rect, Core::Math::RGB::Red);

                Core::Common::FixedString1024 s;
                s.Format(HE_STR_TEXT("FPS(%d)"), HE_ENGINE.GetFPS());

                Render::Command2DTextDraw(sceneHandle, Core::Math::Vector2(0.0f, 0.0f), s, 16,
                                          Core::Math::RGB::White, Core::Math::EAnchor_Left, 0);

                return FALSE;
            }

            return TRUE;
        });
}

/*
TEST_CASE("SDL2 Load ImgTexture")
{
    UnitTestRunnerByModuleOnly<AssetManager::AssetManagerModule, PlatformSDL2::PlatformSDL2Module,
                               Render::RenderModule>(
        []()
        {
            auto pPlatformModule = HE_ENGINE.PlatformModule();
            // ゲームウィンドウを作成
            auto spScreen = pPlatformModule->VScreen();
            HE_ASSERT(spScreen);

            auto pAssetManagerModule =
                HE_ENGINE.ModuleManager().Get<AssetManager::AssetManagerModule>();
            pAssetManagerModule->SetCurrentDir(HE_STR_TEXT("Assets"));

            // 画像データのバイナリアセットを作成
            auto handle =
                pAssetManagerModule->Load<AssetManager::AssetDataBinary>("Image",
                                                                         "Img/TestImg.png");
            auto& rAsset = pAssetManagerModule->GetAsset<AssetManager::AssetDataBinary>(handle);

            auto imgHandle = spScreen->VCreateTextureImage(rAsset.Mem(), rAsset.Size());
            CHECK(imgHandle.Null() == FALSE);

            pAssetManagerModule->Unload(handle);

            CHECK(spScreen->VReleaseTexture(imgHandle));

            return TRUE;
        });
}
*/
