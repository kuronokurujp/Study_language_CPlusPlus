#pragma once

#include "Engine/Common/Singleton.h"
#include "Engine/MiniEngine.h"

// デバッグビルドのみ有効
#ifdef HE_ENGINE_DEBUG
#include <functional>
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
    Bool Init();

    /// <summary>
    /// モジュールを作成して追加
    /// </summary>
    template <class T>
    Bool AddModule()
    {
        HE_ASSERT(this->_bInit);

        T* pModule = HE_NEW_MEM_LAST(T, 0);
        if (this->_AddModule(pModule) == FALSE)
        {
            HE_DELETE_MEM(pModule);
            return FALSE;
        }

        return TRUE;
    }

    /// <summary>
    /// エンジン起動
    /// Initメソッドを事前に呼ばないとエラーになる
    /// </summary>
    Bool Start();

    /// <summary>
    /// エンジン破棄
    /// </summary>
    Bool VRelease() override final;

    /// <summary>
    /// メインゲームウィンドウ生成
    /// </summary>
    Bool CreateMainWindow();

    /// <summary>
    /// ゲームウィンドウを解放
    /// </summary>
    void ReleseWindows();

    // エンジンを稼働させるためのループ用メソッド
    Bool BeforeUpdateLoop(const Float32 in_fDt);
    Bool WaitFrameLoop();
    Bool MainUpdateLoop(const Float32 in_fDt);
    Bool LateUpdateLoop(const Float32 in_fDt);

    /// <summary>
    /// メモリ管理を取得
    /// </summary>
    Core::Memory::Manager& MemManager() { return this->_memoryManager; }

    /// <summary>
    /// モジュール管理を取得
    /// </summary>
    Module::ModuleManager& ModuleManager() { return *this->_upModuleManager.get(); }

    /// <summary>
    /// デバッグモードかどうか
    /// </summary>
    /// <returns></returns>
    inline Bool IsDebugMode() const
    {
#ifdef _DEBUG
        return TRUE;
#endif
        return FALSE;
    }

    /// <summary>
    /// １フレームの差分時間を秒で取得
    /// </summary>
    Float32 GetDeltaTimeSec();

    /// <summary>
    /// アプリを辞める状態か
    /// </summary>
    Bool IsAppQuit();

private:
    /// <summary>
    /// プラットフォームのモジュールを取得
    /// windows / android / iosなどのプラットフォームが扱える予定
    /// 現在はwindowsのみ
    /// </summary>
    Platform::PlatformModule* _PlatformModule();

    /// <summary>
    /// モジュール追加
    /// </summary>
    Bool _AddModule(Module::ModuleBase* in_pModule);

private:
    Bool _bInit  = FALSE;
    Bool _bStart = FALSE;

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
#include <functional>

/// <summary>
/// 一つのモジュールを単体テスト実行
/// </summary>
template <typename T>
static void UnitTestRunnerByModuleOnly(std::function<void(T*)> in_func)
{
    // エンジン起動
    HE_CREATE_ENGINE;

    const Bool bPreInitRet = HE_ENGINE.Init();
    HE_ASSERT(bPreInitRet && "事前初期化に失敗");

    // テストモジュールを登録
    {
        HE_ENGINE.AddModule<T>();
    }

    const Bool bInitRet = HE_ENGINE.Start();
    HE_ASSERT(bInitRet && "初期化に失敗");
    {
        const Float32 d = HE_ENGINE.GetDeltaTimeSec();
        if (HE_ENGINE.BeforeUpdateLoop(d))
        {
            const Float32 d = HE_ENGINE.GetDeltaTimeSec();

            if (HE_ENGINE.MainUpdateLoop(d))
            {
                HE_ENGINE.LateUpdateLoop(d);
                // TODO: ここでモジュールのテストをする?
                in_func(HE_ENGINE.ModuleManager().Get<T>());
            }
        }
    }

    // エンジン終了
    HE_DELETE_ENGINE;
}
#endif
#endif
