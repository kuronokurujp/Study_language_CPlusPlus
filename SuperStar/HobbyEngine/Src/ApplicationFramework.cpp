#include "Engine/ApplicationFramework.h"

#include "Engine/Engine.h"

HE::Bool ApplicationEngineFramework::Init(const HE::Bool in_bDebug)
{
    // エンジン起動
    HE_CREATE_ENGINE;

    // TODO: エンジン用の設定ファイルが必要

    const HE::Bool bPreInitRet = HE_ENGINE.Init();
    HE_ASSERT(bPreInitRet && "事前初期化に失敗");

    // モジュール登録
    if (this->_VRegistEngineModule() == FALSE) return FALSE;

    const HE::Bool bInitRet = HE_ENGINE.Start();
    HE_ASSERT(bInitRet && "初期化に失敗");
    if (bInitRet == FALSE) return FALSE;

    return TRUE;
}

void ApplicationEngineFramework::Release()
{
    this->_VUnRegistEngineModule();

    // エンジン終了
    HE_DELETE_ENGINE;
}

void ApplicationEngineFramework::Running()
{
    this->_VLoad();

    // TODO: 非同期処理にしたい
    this->_VStart();

    // ゲームループ
    HE::Float32 fDelta = 0.0f;
    while (HE_ENGINE.IsQuit() == FALSE)
    {
        // 前処理
        fDelta = HE_ENGINE.GetDeltaTimeSec();
        HE_ENGINE.BeforeUpdateLoop(fDelta);
        if (HE_ENGINE.IsQuit()) break;

        HE_ENGINE.WaitFrameLoop();
        if (HE_ENGINE.IsQuit()) break;

        HE_ENGINE.MainUpdateLoop(fDelta);
        if (HE_ENGINE.IsQuit()) break;

        HE_ENGINE.LateUpdateLoop(fDelta);
    }

    this->_VEnd();
}
