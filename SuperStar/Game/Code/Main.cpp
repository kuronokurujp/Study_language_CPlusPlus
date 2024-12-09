// アプリのエントリーポイント

#include "Main.h"

#include "Common.h"
#include "Engine/Engine.h"

// 外部モジュール一覧
#include "ActorModule.h"
#include "AssetManagerModule.h"
// #include "DXLibModule.h"
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

    // アプリ開始
    if (pAppEngineFramework->VStart(TRUE) == FALSE) return 0;

    // ゲームループ
    pAppEngineFramework->VGameLoop();

    // アプリ終了
    pAppEngineFramework->VEnd();

    return 0;
}

#ifdef HE_WIN

WinGameMain::WinGameMain(HINSTANCE in_instance, HE::Int in_cmdShow)
{
    this->_inst    = in_instance;
    this->_cmdShow = in_cmdShow;
}

HE::Bool WinGameMain::VStart(const HE::Bool in_bDebug)
{
    // アプリに関連する情報を取得
    LoadStringW(this->_inst, IDS_APP_TITLE, _szTitle, uMaxLoadStringLength);
    LoadStringW(this->_inst, IDC_SUPERSTAR, _szWindowClass, uMaxLoadStringLength);

    if (ApplicationEngineFramework::VStart(in_bDebug) == FALSE) return FALSE;

    // ゲームウィンドウを作成
    auto pPlatformModule = HE_ENGINE.PlatformModule();
    HE_ASSERT(pPlatformModule);

    auto upScreen = pPlatformModule->VScreen();
    HE_ASSERT(upScreen);

    Core::Common::Handle windowHandle;
    Core::Common::Handle viewPortHandle;
    {
        // TODO: 外部設定が必要かも
        // ゲームウィンドウを生成
        windowHandle = upScreen->VCreateWindow();

        // TODO: 画面に表示するビューポート
        // ゲームウィンドウで利用するビューポートを追加
        viewPortHandle = upScreen->VAddViewPort(windowHandle);
    }

    // ユーザー共通入力割り当て設定
    {
        EnhancedInput::ActionData::ActionKeyMap aKeys({Platform::EKeyboard::EKeyboard_A});
        EnhancedInput::ActionData::ActionTouchMap aTouchs(
            {Platform::EInputMouseType::EInputMouseType_Left});

        EnhancedInput::ActionMap mInputAction;
        mInputAction.Add(HE_STR_TEXT("UIButton"), EnhancedInput::ActionData(aKeys, aTouchs));

        auto pInputModule = HE_ENGINE.ModuleManager().Get<EnhancedInput::EnhancedInputModule>();
        pInputModule->SetCommonMappingAction(mInputAction);
    }

    // リソースの起点ディレクトリを設定
    auto pAssetManagerModule = HE_ENGINE.ModuleManager().Get<AssetManager::AssetManagerModule>();
    pAssetManagerModule->SetMountDir(HE_STR_TEXT("Assets"));

    auto pLocateModule = HE_ENGINE.ModuleManager().Get<Localization::LocalizationModule>();
    pLocateModule->LoadSystemFile(Core::Common::FixedString256(HE_STR_TEXT("Locate/System.toml")));
    pLocateModule->LoadTextAll(Core::Common::FixedString16(HE_STR_TEXT("JP")));

    // TODO: ゲーム画面表示準備
    {
        // TODO: 2Dのゲームシーンを追加
        Platform::ScreenSceneView2DConfig config2D(windowHandle, viewPortHandle);
        Game::g_scene2DHandle = upScreen->VAddSceneView2D(config2D);
        HE_ASSERT(Game::g_scene2DHandle.Null() == FALSE);

        // TODO: UIのゲームシーンを追加
        Platform::ScreenSceneViewUIConfig configUI(windowHandle, viewPortHandle);
        Game::g_sceneUIHandle = upScreen->VAddSceneViewUI(configUI);
        HE_ASSERT(Game::g_sceneUIHandle.Null() == FALSE);

        // ゲームウィンドウを表示
        upScreen->VShowWindow(windowHandle);
    }

    auto pLevelModule = HE_ENGINE.ModuleManager().Get<Level::LevelModule>();
#ifdef HE_ENGINE_DEBUG
    // デバッグレベルを開始
    pLevelModule->GetManager()->StartLevel<Level::LevelLauncher>();
#else
    pLevelModule->GetManager()->StartLevel<Level::LevelTitle>();
#endif

    return TRUE;
}

HE::Bool WinGameMain::VEnd()
{
    return ApplicationEngineFramework::VEnd();
}

HE::Bool WinGameMain::_VRegistEngineModule()
{
    // HE_ENGINE.AddModule<DXLib::DXLibModule>();
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

    return TRUE;
}

HE::Bool WinGameMain::_VUnRegistEngineModule()
{
    return TRUE;
}

#endif
