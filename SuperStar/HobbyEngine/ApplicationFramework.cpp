#include "ApplicationFramework.h"

#include "Engine/Engine.h"

HE::Bool ApplicationEngineFramework::VStart(const HE::Bool in_bDebug)
{
    HE_LOG_LINE(HE_STR_TEXT("game start"));

    // エンジン起動
    HE_CREATE_ENGINE;

    // TODO: エンジン用の設定ファイルが必要

    const HE::Bool bPreInitRet = HE_ENGINE.Init();
    HE_ASSERT(bPreInitRet && "事前初期化に失敗");

    // TODO: モジュール登録
    if (this->_VRegistEngineModule() == FALSE) return FALSE;

    const HE::Bool bInitRet = HE_ENGINE.Start();
    HE_ASSERT(bInitRet && "初期化に失敗");
    if (bInitRet == FALSE) return FALSE;

    return TRUE;
}

HE::Bool ApplicationEngineFramework::VGameLoop()
{
    // ゲームループ
    HE::Float32 fDelta = 0.0f;
    while (HE_ENGINE.IsAppQuit() == FALSE)
    {
        // 前処理
        fDelta = HE_ENGINE.GetDeltaTimeSec();
        if (HE_ENGINE.BeforeUpdateLoop(fDelta) == FALSE) break;

        if (HE_ENGINE.WaitFrameLoop() == FALSE)
        {
            break;
        }

        if (HE_ENGINE.MainUpdateLoop(fDelta) == FALSE) break;

        if (HE_ENGINE.LateUpdateLoop(fDelta) == FALSE) break;
    }

    return TRUE;
}

HE::Bool ApplicationEngineFramework::VEnd()
{
    HE_LOG_LINE(HE_STR_TEXT("game end"));

    this->_VUnRegistEngineModule();

    HE_DELETE_ENGINE;

    return TRUE;
}
