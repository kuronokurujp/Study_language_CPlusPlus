// アプリのエントリーポイント

#include "Main.h"

#include "Common.h"
#include "Engine/Engine.h"

// 外部モジュール一覧
#include "ActorModule.h"
#include "AssetManagerModule.h"
#include "EnhancedInputModule.h"
#include "EventModule.h"
#include "LevelModule.h"
#include "LocalizationModule.h"
#include "LuaModule.h"
#include "PlatformSDL2Module.h"
#include "RenderModule.h"
#include "UIModule.h"

#ifdef HE_ENGINE_DEBUG

// 最初に実行するレベルをインクルード
#include "Level/LevelLauncher.h"

// デバッグGUI
#include "GameDevGUIModule.h"

#else
#include "Level/LevelTitle.h"
#endif

// Windowsのエントリーポイント
#ifdef HE_WIN

#include "Platform/Win/Framework.h"
#include "Platform/Win/Resource.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    WinGameMain appFramework(hInstance, nCmdShow);
// 各プラットフォームのエントリーポイントは追加予定
#else
// TODO: 未実装
int main()
{
#endif
    ApplicationEngineFramework* pAppEngineFramework = &appFramework;
#ifdef HE_ENGINE_DEBUG
    pAppEngineFramework->Init(TRUE);
#else
    pAppEngineFramework->Init(FALSE);
#endif

    pAppEngineFramework->Running();

    pAppEngineFramework->Release();

    ::TerminateProcess(::GetCurrentProcess(), 0);
    return 0;
}

#ifdef HE_WIN

WinGameMain::WinGameMain(HINSTANCE in_instance, HE::Int in_cmdShow)
{
    this->_inst    = in_instance;
    this->_cmdShow = in_cmdShow;
}

HE::Bool WinGameMain::_VLoad()
{
    // アプリに関連する情報を取得
    LoadStringW(this->_inst, IDS_APP_TITLE, _szTitle, uMaxLoadStringLength);
    LoadStringW(this->_inst, IDC_SUPERSTAR, _szWindowClass, uMaxLoadStringLength);

    auto pPlatformModule = HE_ENGINE.PlatformModule();
    HE_ASSERT_RETURN_VALUE(FALSE, pPlatformModule);

    // リソースの起点ディレクトリを設定
    auto pAssetManagerModule = HE_ENGINE.ModuleManager().Get<AssetManager::AssetManagerModule>();
    pAssetManagerModule->SetCurrentDir(HE_STR_TEXT("Assets"));

    auto pLocateModule = HE_ENGINE.ModuleManager().Get<Localization::LocalizationModule>();
    pLocateModule->LoadSystemFile(Core::Common::FixedString256(HE_STR_TEXT("Locate/System.toml")));
    pLocateModule->LoadTextAll(Core::Common::FixedString16(HE_STR_TEXT("JP")));

    // フォント用意
    {
        auto spFont = pPlatformModule->VFont();
        spFont->VLoad(Platform::EFontSize::EFontSize_64, HE_STR_TEXT("Font/Font.ttf"));
    }

    // ユーザー共通入力割り当て設定
    {
        EnhancedInput::ActionData::ActionKeyMap aKeys({Platform::EKeyboard::EKeyboard_A});
        EnhancedInput::ActionData::ActionTouchMap aTouchs(
            {Platform::EInputMouseType::EInputMouseType_Left});

        EnhancedInput::ActionMap mInputAction;
        mInputAction.Add(HE_STR_TEXT("UIButton"), EnhancedInput::ActionData(aKeys, aTouchs));

        auto pInputModule = HE_ENGINE.ModuleManager().Get<EnhancedInput::EnhancedInputModule>();
        pInputModule->SetAction(mInputAction);
    }

    return TRUE;
}

HE::Bool WinGameMain::_VStart()
{
    // ゲームウィンドウを作成
    auto pRenderModule = HE_ENGINE.ModuleManager().Get<Render::RenderModule>();
    Core::Common::Handle windowHandle;
    Core::Common::Handle viewPortHandle;
    {
        // ゲームウィンドウを生成
        windowHandle = pRenderModule->NewWindow(
            [](Core::Common::Handle in_handle)
            {
                // ウィンドウを作るための設定
                auto pPlatformModule = HE_ENGINE.PlatformModule();
                auto inputHandle     = pPlatformModule->VInput()->VCreateObject();

                Platform::WindowConfig platformWindowConfig(640, 480, 1, TRUE, inputHandle);
                /*
                {
                    platformWindowConfig._uWidth         = 640;
                    platformWindowConfig._uHeight        = 480;
                    platformWindowConfig._uViewPortCount = 1;
                    platformWindowConfig._bMain          = TRUE;
                    platformWindowConfig._inputHandle    = inputHandle;
                }
                */
                return pPlatformModule->VScreen()->VCreateWindowStrategy(in_handle,
                                                                         platformWindowConfig);
            });

        // TODO: 画面に表示するビューポート
        // ゲームウィンドウで利用するビューポートを追加
        viewPortHandle = pRenderModule->AddViewPort(windowHandle, 640, 480);
    }

    // ゲーム画面表示準備
    {
        // 2Dのゲームシーンを追加
        auto [scene2DHandle, p2DScene] =
            pRenderModule->AddSceneView(windowHandle, viewPortHandle,
                                        []()
                                        {
                                            auto pPlatformModule = HE_ENGINE.PlatformModule();

                                            return pPlatformModule->VScreen()
                                                ->VCreateScene2DStrategy();
                                        });
        HE_ASSERT(scene2DHandle.Null() == FALSE);
        Game::g_scene2DHandle = scene2DHandle;

        // UIのゲームシーンを追加
        auto [sceneUIHandle, pUIScene] =
            pRenderModule->AddSceneView(windowHandle, viewPortHandle,
                                        []()
                                        {
                                            auto pPlatformModule = HE_ENGINE.PlatformModule();

                                            return pPlatformModule->VScreen()
                                                ->VCreateSceneUIStrategy();
                                        });

        HE_ASSERT(sceneUIHandle.Null() == FALSE);
        Game::g_sceneUIHandle = sceneUIHandle;

        // ゲームウィンドウを表示
        pRenderModule->ShowWindow(windowHandle);
    }

    // TODO: ウィンドウの入力を拡張インプットを設定
    {
        auto pEnhancedInputModule =
            HE_ENGINE.ModuleManager().Get<EnhancedInput::EnhancedInputModule>();

        auto* pMainWindow = pRenderModule->GetWindow(windowHandle);
        pEnhancedInputModule->SetInputHandle(pMainWindow->GetConfig()->InputHandle());
    }

    auto pLevelModule = HE_ENGINE.ModuleManager().Get<Level::LevelModule>();
#ifdef HE_ENGINE_DEBUG
    // デバッグレベルを開始
    pLevelModule->ChangeMainLevel<Level::LevelLauncher>();
#else
    pLevelModule->ChangeMainLevel<Level::LevelTitle>();
#endif

    return TRUE;
}

HE::Bool WinGameMain::_VEnd()
{
    return TRUE;
}

HE::Bool WinGameMain::_VRegistEngineModule()
{
    HE_ENGINE.AddModule<PlatformSDL2::PlatformSDL2Module>();
    HE_ENGINE.AddModule<Render::RenderModule>();
    HE_ENGINE.AddModule<Actor::ActorModule>();
    HE_ENGINE.AddModule<UI::UIModule>();
    HE_ENGINE.AddModule<AssetManager::AssetManagerModule>();
    HE_ENGINE.AddModule<Level::LevelModule>();
    HE_ENGINE.AddModule<Localization::LocalizationModule>();
    HE_ENGINE.AddModule<EnhancedInput::EnhancedInputModule>();
    HE_ENGINE.AddModule<Event::EventModule>();
    HE_ENGINE.AddModule<Lua::LuaModule>();

#ifdef HE_ENGINE_DEBUG

    HE_ENGINE.AddModule<GameDevGUI::GameDevGUIModule>();

#endif

    return TRUE;
}

HE::Bool WinGameMain::_VUnRegistEngineModule()
{
    return TRUE;
}

#endif
