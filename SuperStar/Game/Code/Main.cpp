// アプリのエントリーポイント

#include "Main.h"

#include "Engine/Engine.h"

#ifdef HE_WIN

#include "Platform/Win/Framework.h"
#include "Platform/Win/Resource.h"

#endif

// 外部モジュール一覧
#include "ActorModule.h"
#include "AssetManagerModule.h"
#include "DXLibModule.h"
#include "EnhancedInputModule.h"
#include "EventModule.h"
#include "LevelModule.h"
#include "LocalizationModule.h"
#include "LuaModule.h"
#include "RenderModule.h"
#include "UIModule.h"

#ifdef HE_WIN

#define MAX_LOADSTRING 100

// グローバル変数

// 現在のインターフェイス
static HINSTANCE hInst;

// タイトル バーのテキスト
static WCHAR szTitle[MAX_LOADSTRING];

// メイン ウィンドウ クラス名
static WCHAR szWindowClass[MAX_LOADSTRING];

// ゲームエンジンに登録するアプリインスタンス
static AppEntryGameMain s_app;

// このコード モジュールに含まれる関数の宣言を転送します:
static BOOL InitInstance(HINSTANCE, int);
static void EndInstance(HINSTANCE hInstance);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // アプリに関連する情報を取得
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SUPERSTAR, szWindowClass, MAX_LOADSTRING);

    // アプリケーション初期化の実行:
    if (InitInstance(hInstance, nCmdShow))
    {
        // エンジンに設定したアプリを開始
        s_app.VStart(HE_ENGINE.IsDebugMode());

        // ゲームループ
        Float32 fDelta = 0.0f;
        while (HE_ENGINE.IsAppQuit() == FALSE)
        {
            // 前処理
            fDelta = HE_ENGINE.GetDeltaTimeSec();
            {
                if (HE_ENGINE.BeforeUpdateLoop(fDelta) == FALSE) break;
            }

            if (HE_ENGINE.WaitFrameLoop() == FALSE)
            {
                break;
            }

            {
                // アプリメイン
                if (s_app.VUpdate(fDelta) == FALSE) break;

                if (HE_ENGINE.MainUpdateLoop(fDelta) == FALSE) break;

                if (HE_ENGINE.LateUpdateLoop(fDelta) == FALSE) break;
            }
        }

        s_app.VEnd();
    }

    EndInstance(hInstance);

    return 0;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    // グローバル変数にインスタンス ハンドルを格納する
    hInst = hInstance;

    // エンジン起動
    HE_CREATE_ENGINE;

    // TODO: エンジン用の設定ファイルが必要

    const Bool bPreInitRet = HE_ENGINE.Init();
    HE_ASSERT(bPreInitRet && "事前初期化に失敗");

    // 利用モジュールを登録
    {
        HE_ENGINE.AddModule<DXLib::DXLibModule>();
        HE_ENGINE.AddModule<Render::RenderModule>();
        HE_ENGINE.AddModule<Actor::ActorModule>();
        HE_ENGINE.AddModule<UI::UIModule>();
        HE_ENGINE.AddModule<AssetManager::AssetManagerModule>();
        HE_ENGINE.AddModule<Level::LevelModule>();
        HE_ENGINE.AddModule<Localization::LocalizationModule>();
        HE_ENGINE.AddModule<EnhancedInput::EnhancedInputModule>();
        HE_ENGINE.AddModule<Event::EventModule>();
        HE_ENGINE.AddModule<Lua::LuaModule>();
    }

    const Bool bInitRet = HE_ENGINE.Start();
    HE_ASSERT(bInitRet && "初期化に失敗");

    // ゲームウィンドウを作成
    if (HE_ENGINE.CreateMainWindow() == FALSE) return FALSE;

    return TRUE;
}

void EndInstance(HINSTANCE hInstance)
{
    HE_DELETE_ENGINE;
}

#endif

// アプリの起動エントリークラス

#ifdef HE_ENGINE_DEBUG

// 最初に実行するレベルをインクルード
#include "Level/LevelLauncher.h"

#else
#include "Level/LevelTitle.h"
#endif

Bool AppEntryGameMain::VStart(const Bool in_bDebug)
{
    HE_LOG_LINE(HE_STR_TEXT("game start"));

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

    // TODO: ゲームのみで利用するライブラリを初期化
    //		LuaStateManager::Init();

    // リソースの起点ディレクトリを設定
    auto pAssetManagerModule = HE_ENGINE.ModuleManager().Get<AssetManager::AssetManagerModule>();
    pAssetManagerModule->SetMountDir(HE_STR_TEXT("Assets"));

    auto pLocateModule = HE_ENGINE.ModuleManager().Get<Localization::LocalizationModule>();
    pLocateModule->LoadSystemFile(Core::Common::FixString256(HE_STR_TEXT("Locate/System.toml")));
    pLocateModule->LoadTextAll(Core::Common::FixString16(HE_STR_TEXT("JP")));

    auto pLevelModule = HE_ENGINE.ModuleManager().Get<Level::LevelModule>();
#ifdef HE_ENGINE_DEBUG
    // デバッグレベルを開始
    pLevelModule->GetManager()->StartLevel<Level::LevelLauncher>();
#else
    pLevelModule->GetManager()->StartLevel<Level::LevelTitle>();
#endif

    return TRUE;
}

Bool AppEntryGameMain::VUpdate(const Float32 in_fDeltaTime)
{
    return TRUE;
}

Bool AppEntryGameMain::VEnd()
{
    HE_LOG_LINE(HE_STR_TEXT("game end"));
    return TRUE;
}
