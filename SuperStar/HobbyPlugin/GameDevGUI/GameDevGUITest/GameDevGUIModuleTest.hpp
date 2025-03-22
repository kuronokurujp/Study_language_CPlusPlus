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

    UnitTestRunnerByModuleOnly<PlatformSDL2::PlatformSDL2Module, Render::RenderModule,
                               GameDevGUI::GameDevGUIModule>(
        [&uStep, &sceneHandle]()
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
                                Platform::WindowConfig platformWindowConfig;
                                {
                                    platformWindowConfig._uWidth         = 640;
                                    platformWindowConfig._uHeight        = 480;
                                    platformWindowConfig._uViewPortCount = 1;
                                    platformWindowConfig._bMain          = TRUE;
                                    platformWindowConfig._inputHandle    = inputHandle;
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
                }

                // サブウィンドウ(ImGUI用)
                {
                    Core::Common::Handle windowHandle;
                    Core::Common::Handle viewPortHandle;
                    {
                        // デバッグウィンドウを生成
                        windowHandle = pRenderModule->NewWindow(
                            [](const Core::Common::Handle in_handle)
                            {
                                auto pGameDevGUIModule =
                                    HE_ENGINE.ModuleManager().Get<GameDevGUI::GameDevGUIModule>();

                                auto pPlatformModule = HE_ENGINE.PlatformModule();
                                auto inputHandle     = pPlatformModule->VInput()->VCreateObject();

                                Platform::WindowConfig platformWindowConfig;
                                {
                                    platformWindowConfig._uWidth         = 320;
                                    platformWindowConfig._uHeight        = 240;
                                    platformWindowConfig._uViewPortCount = 1;
                                    platformWindowConfig._bMain          = FALSE;
                                    platformWindowConfig._inputHandle    = inputHandle;
                                }

                                return pGameDevGUIModule
                                    ->CreateWindowStrategy(in_handle, platformWindowConfig);
                            });

                        // 画面に表示するビューポート
                        // ゲームウィンドウで利用するビューポートを追加
                        viewPortHandle = pRenderModule->AddViewPort(windowHandle, 320, 240);

                        // ウィンドウ座標設定
                        auto pWindow = pRenderModule->GetWindow(windowHandle);
                        pWindow->SetPos(0, 32);
                    }

                    // GameDevUIのゲームシーンを追加
                    {
                        auto [handle, pScene] = pRenderModule->AddSceneView(
                            windowHandle, viewPortHandle,
                            []()
                            {
                                auto pGameDevGUIModule =
                                    HE_ENGINE.ModuleManager().Get<GameDevGUI::GameDevGUIModule>();
                                return pGameDevGUIModule->CreateSceneStrategy();
                            });
                        sceneHandle = handle;
                    }
                    // ゲームウィンドウを表示
                    pRenderModule->ShowWindow(windowHandle);
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
