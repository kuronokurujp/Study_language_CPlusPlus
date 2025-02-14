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
                /*
                    auto pRenderModule = HE_ENGINE.ModuleManager().Get<Render::RenderModule>();
                    pRenderModule->AddEventBeginWindow(
                        [](const Core::Common::Handle in_handle)
                        {
                            auto pGameDevGUIModule =
                                HE_ENGINE.ModuleManager().Get<GameDevGUI::GameDevGUIModule>();
                            pGameDevGUIModule->NewGUI(in_handle);
                        });
                    pRenderModule->AddEventEndWindow(
                        [](const Core::Common::Handle in_handle)
                        {
                            auto pGameDevGUIModule =
                                HE_ENGINE.ModuleManager().Get<GameDevGUI::GameDevGUIModule>();
                            pGameDevGUIModule->DestoryGUI(in_handle);
                        });
                        */

                ++uStep;

                return FALSE;
            }
            else if (uStep == 1)
            {
                // メインウィンドウ
                {
                    Core::Common::Handle windowHandle;
                    Core::Common::Handle viewPortHandle;
                    {
                        // ゲームウィンドウを生成
                        windowHandle = pRenderModule->NewWindow(640, 480, TRUE);

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
                }

                // TODO: サブウィンドウ(ImGUI用)
                {
                    Core::Common::Handle windowHandle;
                    Core::Common::Handle viewPortHandle;
                    {
                        // デバッグウィンドウを生成
                        windowHandle = pRenderModule->NewWindow(
                            320, 240, FALSE,
                            [](Core::Common::Handle in_handle)
                            {
                                auto pGameDevGUIModule =
                                    HE_ENGINE.ModuleManager().Get<GameDevGUI::GameDevGUIModule>();
                                pGameDevGUIModule->NewGUI(in_handle);
                            },
                            [](Core::Common::Handle in_handle)
                            {
                                auto pGameDevGUIModule =
                                    HE_ENGINE.ModuleManager().Get<GameDevGUI::GameDevGUIModule>();
                                pGameDevGUIModule->DestoryGUI();
                            });

                        // 画面に表示するビューポート
                        // ゲームウィンドウで利用するビューポートを追加
                        viewPortHandle = pRenderModule->AddViewPort(windowHandle, 32, 240);

                        // ウィンドウ座標設定
                        auto pWindow = pRenderModule->GetWindow(windowHandle);
                        pWindow->SetPos(0, 32);
                    }

                    // UIのゲームシーンを追加
                    {
                        auto [handle, pScene] =
                            pRenderModule->AddSceneViewUI(windowHandle, viewPortHandle);
                        sceneHandle = handle;
                    }
                    // ゲームウィンドウを表示
                    pRenderModule->ShowWindow(windowHandle);
                }

                ++uStep;

                return FALSE;
            }
            else if (uStep == 2)
            {
                // ウィンドウが閉じたら終了

                return FALSE;
            }

            return TRUE;
        });
}
