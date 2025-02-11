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

                return FALSE;
            }

            return TRUE;
        });
}
