#pragma once

#include "Engine/Common/Singleton.h"
#include "Engine/MiniEngine.h"

// デバッグビルドのみ有効
#ifdef HE_ENGINE_DEBUG
#ifdef HE_UNIT_TEST_MODE
#include <functional>
#endif
#endif

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
    /// モジュールを作成して追加
    /// </summary>
    template <class T>
    HE::Bool AddModule()
    {
        HE_ASSERT(this->_bInit);

        T* pModule = HE_NEW_MEM_LAST(T, 0);
        if (this->_AddModule(pModule) == FALSE)
        {
            HE_SAFE_DELETE_MEM(pModule);
            return FALSE;
        }

        return TRUE;
    }

    /// <summary>
    /// エンジン起動
    /// Initメソッドを事前に呼ばないとエラーになる
    /// </summary>
    HE::Bool Start();

    /// <summary>
    /// エンジン破棄
    /// </summary>
    HE::Bool VRelease() override final;

    // エンジンを稼働させるためのループ用メソッド
    HE::Bool BeforeUpdateLoop(const HE::Float32);
    HE::Bool WaitFrameLoop();
    HE::Bool MainUpdateLoop(const HE::Float32);
    HE::Bool LateUpdateLoop(const HE::Float32);

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
    Platform::PlatformModule* PlatformModule();

    /// <summary>
    /// デバッグモードかどうか
    /// </summary>
    /// <returns></returns>
    inline HE::Bool IsDebugMode() const
    {
#ifdef _DEBUG
        return TRUE;
#endif
        return FALSE;
    }

    /// <summary>
    /// １フレームの差分時間を秒で取得
    /// </summary>
    HE::Float32 GetDeltaTimeSec();

    /// <summary>
    /// アプリを辞める状態か
    /// </summary>
    HE::Bool IsAppQuit();

private:
    /// <summary>
    /// モジュール追加
    /// </summary>
    HE::Bool _AddModule(Module::ModuleBase* in_pModule);

private:
    HE::Bool _bInit  = FALSE;
    HE::Bool _bStart = FALSE;

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
        s_engine.Set();         \
    }
// エンジン削除
#define HE_DELETE_ENGINE     \
    {                        \
        Engine::I().Reset(); \
    }

// デバッグビルドのみ有効
#ifdef HE_ENGINE_DEBUG
// ユニットテスト専用
#ifdef HE_UNIT_TEST_MODE

/// <summary>
/// 一つのモジュールを単体テスト実行
/// </summary>
template <class... TArgs>
static void UnitTestRunnerByModuleOnly(std::function<HE::Bool()> in_func)
{
    // エンジン起動
    HE_CREATE_ENGINE;

    const HE::Bool bPreInitRet = HE_ENGINE.Init();
    HE_ASSERT(bPreInitRet && "事前初期化に失敗");

    // テストモジュールを登録
    {
        (HE_ENGINE.AddModule<TArgs>(), ...);
    }

    const HE::Bool bInitRet = HE_ENGINE.Start();
    HE_ASSERT(bInitRet && "初期化に失敗");
    HE::Bool bEnd = FALSE;
    while (bEnd == FALSE)
    {
        const HE::Float32 d = HE_ENGINE.GetDeltaTimeSec();
        if (HE_ENGINE.BeforeUpdateLoop(d))
        {
            const HE::Float32 d = HE_ENGINE.GetDeltaTimeSec();

            if (HE_ENGINE.MainUpdateLoop(d))
            {
                HE_ENGINE.LateUpdateLoop(d);
                // モジュールのテスト
                bEnd = in_func();
            }
        }
    }

    // エンジン終了
    HE_DELETE_ENGINE;
}
#endif
#endif
