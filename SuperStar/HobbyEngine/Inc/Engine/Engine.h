#pragma once

#include "Engine/Common/Singleton.h"
#include "Engine/MiniEngine.h"

// 前方宣言
namespace Core::Time
{
    class FPS;
}

namespace Platform
{
    class PlatformModule;
}

namespace Module
{
    class ModuleManager;
    class ModuleBase;
}  // namespace Module

/// <summary>
/// ゲームエンジン本体
/// インスタンスは一つのみなのでシングルトンにする
/// </summary>
class Engine final : public Core::Common::Singleton<Engine>
{
public:
    /// <summary>
    /// 初期化
    /// メモリシステムなどエンジンを動かす上で最低元の初期化をする
    /// </summary>
    HE::Bool Init();

    /// <summary>
    /// エンジン破棄
    /// </summary>
    HE::Bool VRelease() override final;

    /// <summary>
    /// モジュールを作成して追加
    /// </summary>
    template <class T>
    HE::Bool AddModule()
    {
        HE_ASSERT(this->_bInit);

        auto spModule = HE_MAKE_CUSTOM_SHARED_PTR(T);
        return this->_upModuleManager->Add(spModule);
    }

    /// <summary>
    /// エンジン起動
    /// Initメソッドを事前に呼ばないとエラーになる
    /// </summary>
    HE::Bool Start();

    // エンジンを稼働させるためのループ用メソッド
    void BeforeUpdateLoop(const HE::Float32);
    void WaitFrameLoop();
    void MainUpdateLoop(const HE::Float32);
    void LateUpdateLoop(const HE::Float32);

    /// <summary>
    /// メモリ管理を取得
    /// </summary>
    Core::Memory::Manager& MemManager() { return this->_memoryManager; }

    /// <summary>
    /// モジュール管理を取得
    /// </summary>
    Module::ModuleManager& ModuleManager() { return *this->_upModuleManager.get(); }

    /// <summary>
    /// プラットフォームのモジュールを取得
    /// windows / android / iosなどのプラットフォームが扱える予定
    /// 現在はwindowsのみ
    /// </summary>
    Core::Memory::SharedPtr<Platform::PlatformModule> PlatformModule();

    /// <summary>
    /// １フレームの差分時間を秒で取得
    /// </summary>
    HE::Float32 GetDeltaTimeSec();

    /// <summary>
    /// FPS取得
    /// </summary>
    HE::Uint32 GetFPS();

    /// <summary>
    /// エンジンが終了設定しているか
    /// </summary>
    const HE::Bool IsQuit();

    /// <summary>
    /// エンジン終了設定
    /// </summary>
    void Quit();

private:
    HE::Bool _bInit  = FALSE;
    HE::Bool _bStart = FALSE;
    HE::Bool _bQuit  = FALSE;

    // メモリ管理
    Core::Memory::Manager _memoryManager;

    // FPS 制御
    Core::Memory::SharedPtr<Core::Time::FPS> _spFPS = NULL;

    // モジュール管理
    Core::Memory::UniquePtr<Module::ModuleManager> _upModuleManager = NULL;
};

// エンジン参照マクロ
#define HE_ENGINE Engine::I()

// エンジン作成
#define HE_CREATE_ENGINE        \
    {                           \
        static Engine s_engine; \
        s_engine.Setup();       \
    }
// エンジン削除
#define HE_DELETE_ENGINE     \
    {                        \
        Engine::I().Reset(); \
    }
