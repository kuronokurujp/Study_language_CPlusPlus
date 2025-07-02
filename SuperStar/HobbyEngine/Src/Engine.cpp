#include "Engine/Engine.h"

#include "Engine/Memory/Memory.h"
#include "Engine/Module/Module.h"
#include "Engine/Platform/PlatformModule.h"
#include "Engine/Time/FPS.h"

/// <summary>
/// 事前初期化
/// </summary>
HE::Bool Engine::Init()
{
    HE_ASSERT(this->_bInit == FALSE);
    if (this->_bInit) return TRUE;

#ifdef HE_WIN
#ifdef HE_CHARACTER_CODE_UTF8
    // UTF-8 を使うようにコードページを設定
    // ウィンドウズだが文字コードはUTF8を使うのでwchar_tが使えないのでコンソール設定をUTF8にして日本語表示を文字化けしないようにする
    SetConsoleOutputCP(CP_UTF8);
#endif
#endif

    HE_LOG_LINE(HE_STR_TEXT("エンジンの前準備"));

    this->_memoryManager.Setup();

    // メモリ管理
    // カスタムメモリ確保
    // TODO: 確保数は適当にしている
    // TODO: OSのメモリアロケーターを設定
    // TODO: 使う側が決めるようにしないといけない
    if (this->_memoryManager.Start(1024 * 1024 * 300) == FALSE)
    {
        HE_ASSERT(0 && "カスタムメモリの初期化に失敗");
        return FALSE;
    }

    this->_bQuit = FALSE;
    this->_bInit = TRUE;

    // メモリページ確保
    // TODO: 確保量は適当にしている
    {
        // メモリサイズのイニシャライズ
        Core::Memory::Manager::PageSetupInfo memoryPageSetupInfoArray[] = {
            {// メモリページNo
             0,
             // 利用するメモリ数
             1024 * 1024 * 300},
        };

        if (this->_memoryManager.SetupMemoryPage(memoryPageSetupInfoArray,
                                                 HE_ARRAY_NUM(memoryPageSetupInfoArray)) == FALSE)
        {
            HE_ASSERT(0 && "カスタムメモリのページ作成に失敗");
            return FALSE;
        }

        HE_ASSERT(this->_memoryManager.CheckAllMemoryBlock());
    }

    // モジュール管理を使えるようにする
    this->_upModuleManager = HE_MAKE_CUSTOM_UNIQUE_PTR((Module::ModuleManager));

    // 成功
    return this->_bInit;
}

/// <summary>
/// 起動
/// </summary>
HE::Bool Engine::Start()
{
    HE_ASSERT(this->_bStart == FALSE);
    if (this->_bStart) return TRUE;

    HE_LOG_LINE(HE_STR_TEXT("エンジンの起動"));

    // モジュール管理を使えるようにする
    this->_upModuleManager->Start();

    // FPSタイマーを作成
    // ゲームを固定フレームレートにするため
    auto pPlatformModule = this->PlatformModule();
    if (pPlatformModule)
    {
        this->_spFPS = HE_MAKE_CUSTOM_SHARED_PTR((Core::Time::FPS), pPlatformModule->Time());
        // TODO: FPSは固定にして60にいったんしている
        this->_spFPS->EnableFixedMode(60);
    }

    this->_bStart = TRUE;

    // 成功
    return this->_bStart;
}

HE::Bool Engine::VRelease()
{
    HE_ASSERT(this->_bInit);
    HE_ASSERT(this->_bStart);

    if (this->_bStart == FALSE) return TRUE;
    if (this->_bInit == FALSE) return TRUE;

    // 確保したメモリを解放しないとEngineのデストラクタでエラーになる
    this->_upModuleManager->Release();
    this->_upModuleManager.reset();

    this->_spFPS.reset();

    // アプリ/エンジンで利用しているメモリはここで解放
    // 確保したメモリが残っている場合はエラーになるので注意
    this->_memoryManager.VRelease();
    this->_memoryManager.Reset();

    HE_LOG_LINE(HE_STR_TEXT("エンジンの終了"));
    // デストラクタが呼ばれる
    // メモリ管理から取得したメモリを参照すると例外エラーになる

    this->_bInit  = FALSE;
    this->_bStart = FALSE;
    this->_bQuit  = FALSE;

    return TRUE;
}

void Engine::BeforeUpdateLoop(const HE::Float32 in_fDt)
{
    this->_upModuleManager->BeforeUpdate(in_fDt);
}

void Engine::WaitFrameLoop()
{
    auto pPlatform = this->PlatformModule();
    if (pPlatform == NULL) return;

    if (this->_spFPS == NULL) return;

    HE::Uint64 ulBeginMSec = this->_spFPS->GetLastTimeMSec();

    // 固定フレームモード
    if (this->_spFPS->IsFixedMode())
    {
        // 指定したFPSまで待機
        do
        {
            if (this->_spFPS->IsWaitFrameFixedMode(pPlatform->Time()))
            {
                // TODO: 待機中は何か処理をした方がいい?
                pPlatform->Time()->VSleepMSec(1);
            }
            else
            {
                this->_spFPS->UpdateTime(pPlatform->Time());
                break;
            }
        } while (TRUE);
    }
    else
    {
        this->_spFPS->UpdateTime(pPlatform->Time());
    }

    HE::Uint64 ulEndMSec = this->_spFPS->GetLastTimeMSec();
}

void Engine::MainUpdateLoop(const HE::Float32 in_fDt)
{
    // モジュール更新
    HE_ASSERT(this->_upModuleManager);

    this->_upModuleManager->Update(in_fDt);
}

void Engine::LateUpdateLoop(const HE::Float32 in_fDt)
{
    HE_ASSERT(this->_upModuleManager);

    this->_upModuleManager->LateUpdate(in_fDt);

    // ここでプラットフォームの状態を確認して終了する
    auto spPlatform = this->PlatformModule();
    // エンジンを動かすにはプラットフォームが必要
    if ((spPlatform == NULL) || spPlatform->VIsQuit())
    {
        this->Quit();
    }
}

HE::Float32 Engine::GetDeltaTimeSec()
{
    if (this->_spFPS == NULL) return 0.0f;

    auto pPlatform = this->PlatformModule();
    if (pPlatform == NULL) return 0.0f;

    return this->_spFPS->GetDeltaTimeSec();
}

HE::Uint32 Engine::GetFPS()
{
    if (this->_spFPS == NULL) return 0;
    return this->_spFPS->GetFrameRate();
}

const HE::Bool Engine::IsQuit()
{
    return this->_bQuit;
}

void Engine::Quit()
{
    this->_bQuit = TRUE;
}

Core::Memory::SharedPtr<Platform::PlatformModule> Engine::PlatformModule()
{
    HE_ASSERT(this->_upModuleManager);

    Core::Common::FixedString128 szName(Platform::PlatformModule::ModuleName());
    return HE_SHADER_PTR_CAST(Platform::PlatformModule, this->_upModuleManager->Get(szName.Str()));
}
