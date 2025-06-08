#pragma once

#include "../AssetManager/AssetManagerModule.h"
#include "../EnhancedInput/EnhancedInputModule.h"
#include "../Event/EventModule.h"
#include "../Localization/LocalizationModule.h"
#include "../PlatformSDL2/PlatformSDL2Module.h"
#include "../Render/RenderModule.h"
#include "../UI/UIModule.h"
#include "Engine/Common/Function.h"

// UIテスト
TEST_CASE("UI SimpleTest")
{
    HE::Uint32 uStep = 0;
    Core::Common::Handle sceneHandle;
    Core::Common::Handle debugWindowHandle;
    Core::Common::Handle layoutAssetHandle;
    Core::Common::Handle widget;
    Core::Common::Handle uiEventListenerHash;

    StartupEngineByUnitTest<PlatformSDL2::PlatformSDL2Module, Render::RenderModule, UI::UIModule,
                            Event::EventModule, AssetManager::AssetManagerModule,
                            Localization::LocalizationModule, EnhancedInput::EnhancedInputModule>(
        [&uStep, &sceneHandle, &debugWindowHandle, &layoutAssetHandle, &widget,
         &uiEventListenerHash]()
        {
            auto pPlatformModule = HE_ENGINE.PlatformModule();
            auto pRenderModule   = HE_ENGINE.ModuleManager().Get<Render::RenderModule>();

            if (uStep == 0)
            {
                // インプットにUI用のアクションを追加
                {
                    auto pInputModule =
                        HE_ENGINE.ModuleManager().Get<EnhancedInput::EnhancedInputModule>();
                    // ボタンを押した時のアクション
                    auto szClickLeftName = HE_STR_TEXT("OnClickLeft");
                    pInputModule->AddAction(szClickLeftName,
                                            EnhancedInput::ActionData(
                                                {Platform::EInputMouseType::EInputMouseType_Left}));
                    // UI側はそのアクションが起きた時にUI用のインプットアクション実行
                    auto pUIModule = HE_ENGINE.ModuleManager().Get<UI::UIModule>();
                    pUIModule->AddActiveInputName(szClickLeftName);
                }

                // リソースの起点ディレクトリを設定
                auto pAssetManagerModule =
                    HE_ENGINE.ModuleManager().Get<AssetManager::AssetManagerModule>();
                pAssetManagerModule->SetCurrentDir(HE_STR_TEXT("Assets"));

                // ローカライズテキストをロード
                auto pLocateModule =
                    HE_ENGINE.ModuleManager().Get<Localization::LocalizationModule>();
                pLocateModule->LoadSystemFile(
                    Core::Common::FixedString256(HE_STR_TEXT("Locate/System.toml")));
                pLocateModule->LoadTextAll(Core::Common::FixedString16(HE_STR_TEXT("JP")));

                // フォントデータのバイナリアセットを作成
                {
                    // ロードするフォントファイルパスを渡す
                    // シェーダーファイルも渡す.(プラットフォームによっては使わない)
                    auto bRet = pPlatformModule->VFont()->VLoad(Platform::EFontSize_64,
                                                                HE_STR_TEXT("Font/TestFont.ttf"));
                    CHECK(bRet);
                }

                // メインウィンドウ
                Core::Common::Handle windowHandle;
                {
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

                // 入力設定
                {
                    auto pEnhancedInputModule =
                        HE_ENGINE.ModuleManager().Get<EnhancedInput::EnhancedInputModule>();

                    auto* pMainWindow = pRenderModule->GetWindow(windowHandle);
                    pEnhancedInputModule->SetInputHandle(pMainWindow->GetConfig()->InputHandle());
                }

                // UIのアセットデータをロード
                // UIのBuilderファイルからレイアウト作成
                {
                    auto pUIModule    = HE_ENGINE.ModuleManager().Get<UI::UIModule>();
                    layoutAssetHandle = pUIModule->LoadAssetWithLayoutBuild(
                        Core::File::Path(HE_STR_TEXT("UI"), HE_STR_TEXT("Builder"),
                                         HE_STR_TEXT("Test"), HE_STR_TEXT("Launcher.xml")));
                    // widgetを作成
                    widget = pUIModule->NewLayoutByLayotuAsset(layoutAssetHandle, 0, sceneHandle);
                }

                // UIのイベントリスナー登録
                {
                    auto pEventModule      = HE_ENGINE.ModuleManager().Get<Event::EventModule>();
                    auto spUIEventListener = HE_MAKE_CUSTOM_SHARED_PTR(
                        (Event::EventListenerWithRegistEventFunc), HE_STR_TEXT("UIEvent"),
                        [](Event::EventDataInterfacePtr const& in_spEventData)
                        {
                            // UIのボタンクリック受信
                            if (in_spEventData->VEventHash() == UI::EventButtonClick::Hash())
                            {
                                auto pEvent =
                                    reinterpret_cast<UI::EventButtonClick*>(in_spEventData.get());

                                HE_LOG_LINE(pEvent->_szMsg.Str());
                            }

                            return TRUE;
                        });

                    uiEventListenerHash =
                        pEventModule->AddListener(spUIEventListener, EVENT_TYPE_UIMODULE);
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
