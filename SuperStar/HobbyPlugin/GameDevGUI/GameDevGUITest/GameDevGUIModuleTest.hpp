#pragma once

#define HE_UNIT_TEST_MODE
#include "../GameDevGUI/GameDevGUIModule.h"
#include "../PlatformSDL2/PlatformSDL2Module.h"
#include "../Render/RenderModule.h"
#include "Engine/Common/Function.h"
#include "Engine/Engine.h"

// TODO: GamdDevGUIテスト
TEST_CASE("GameDevGUI Test Open / Close")
{
    HE::Uint32 uStep = 0;
    Core::Common::Handle sceneHandle;
    Core::Common::Handle debugWindowHandle;

    StartupEngineByUnitTest<PlatformSDL2::PlatformSDL2Module, Render::RenderModule,
                            GameDevGUI::GameDevGUIModule>(
        [&uStep, &sceneHandle, &debugWindowHandle]()
        {
            auto pPlatformModule = HE_ENGINE.PlatformModule();
            auto pRenderModule   = HE_ENGINE.ModuleManager().Get<Render::RenderModule>();

            if (uStep == 0)
            {
                // メインウィンドウ
                {
                    Core::Common::Handle windowHandle;
                    Core::Common::Handle viewPortHandle;
                    {
                        // ゲームウィンドウを生成
                        windowHandle = pRenderModule->NewWindow(
                            [](const Core::Common::Handle in_handle)
                            {
                                auto pPlatformModule = HE_ENGINE.PlatformModule();
                                auto inputHandle     = pPlatformModule->VInput()->VCreateObject();
                                Platform::WindowConfig platformWindowConfig(640, 480, 1, TRUE,
                                                                            inputHandle);
                                // TODO: ウィンドウにメニューを追加
                                {
                                    platformWindowConfig.AddMenuItem(10,
                                                                     HE_STR_TEXT(
                                                                         "デバッグ画面を非表示"));
                                    platformWindowConfig.AddMenuItem(9, HE_STR_TEXT(
                                                                            "デバッグ画面を表示"));
                                }

                                return pPlatformModule->VScreen()
                                    ->VCreateWindowStrategy(in_handle, platformWindowConfig);
                            });

                        // 画面に表示するビューポート
                        // ゲームウィンドウで利用するビューポートを追加
                        viewPortHandle = pRenderModule->AddViewPort(windowHandle, 640, 480);
                    }

                    // UIのゲームシーンを追加
                    {
                        auto [handle, pScene] = pRenderModule->AddSceneView(
                            windowHandle, viewPortHandle,
                            // UI用のストラテジーを生成
                            []()
                            {
                                auto pPlatformModule = HE_ENGINE.PlatformModule();
                                return pPlatformModule->VScreen()->VCreateSceneUIStrategy();
                            });
                        sceneHandle = handle;
                    }
                    // ゲームウィンドウを表示
                    pRenderModule->ShowWindow(windowHandle);

                    auto pWindow = pRenderModule->GetWindow(windowHandle);
                    // TODO: ウィンドウのメニュー項目のイベント登録
                    pWindow->RegistEventMenuCallback(
                        [&debugWindowHandle](HE::Uint32 in_uId)
                        {
                            switch (in_uId)
                            {
                                case 9:
                                {
                                    // TODO: メニュー項目を押しているかチェック
                                    auto pRenderModule =
                                        HE_ENGINE.ModuleManager().Get<Render::RenderModule>();
                                    pRenderModule->ShowWindow(debugWindowHandle);

                                    break;
                                }
                                case 10:
                                {
                                    // TODO: メニュー項目を押しているかチェック
                                    auto pRenderModule =
                                        HE_ENGINE.ModuleManager().Get<Render::RenderModule>();
                                    pRenderModule->HideWindow(debugWindowHandle);

                                    break;
                                }
                                default:
                                    break;
                            }
                            // HE_LOG_LINE(HE_STR_TEXT("PushMenuItem: No(%d)"), in_uId);
                        });
                }

                // サブウィンドウ(ImGUI用)
                {
                    Core::Common::Handle viewPortHandle;
                    {
                        // デバッグウィンドウを生成
                        debugWindowHandle = pRenderModule->NewWindow(
                            [](const Core::Common::Handle in_handle)
                            {
                                auto pGameDevGUIModule =
                                    HE_ENGINE.ModuleManager().Get<GameDevGUI::GameDevGUIModule>();

                                auto pPlatformModule = HE_ENGINE.PlatformModule();
                                auto inputHandle     = pPlatformModule->VInput()->VCreateObject();

                                auto eFlag = static_cast<Platform::WindowConfig::EFlags>(
                                    Platform::WindowConfig::EFlags::EFlags_Resizable |
                                    Platform::WindowConfig::EFlags::EFlags_WinDisableCloseBtn);

                                Platform::WindowConfig platformWindowConfig(320, 240, 1, FALSE,
                                                                            inputHandle, eFlag);

                                return pGameDevGUIModule
                                    ->CreateWindowStrategy(in_handle, platformWindowConfig);
                            });

                        // 画面に表示するビューポート
                        // ゲームウィンドウで利用するビューポートを追加
                        viewPortHandle = pRenderModule->AddViewPort(debugWindowHandle, 320, 240);

                        // ウィンドウ座標設定
                        auto pWindow = pRenderModule->GetWindow(debugWindowHandle);
                        pWindow->SetPos(0, 32);
                    }

                    // GameDevUIのゲームシーンを追加
                    {
                        auto [handle, pScene] = pRenderModule->AddSceneView(
                            debugWindowHandle, viewPortHandle,
                            []()
                            {
                                auto pGameDevGUIModule =
                                    HE_ENGINE.ModuleManager().Get<GameDevGUI::GameDevGUIModule>();

                                Core::Memory::UniquePtr<GameDevGUI::GameDevGUISceneStrategy>
                                    sceneSt = pGameDevGUIModule->CreateSceneStrategy();

                                // TODO: カスタムシーンを作成
                                {
                                }

                                return std::move(sceneSt);
                            });
                        sceneHandle = handle;
                    }
                    // ゲームウィンドウを表示
                    pRenderModule->ShowWindow(debugWindowHandle);
                }

                ++uStep;

                return FALSE;
            }
            else if (uStep == 1)
            {
                // ウィンドウが閉じたら終了

                return FALSE;
            }

            return TRUE;
        });
}
