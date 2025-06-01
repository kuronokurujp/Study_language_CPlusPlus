#pragma once

#include <functional>

#include "Engine/Engine.h"

/// <summary>
/// テンプレート型で指定したモジュールをロードしてエンジンを起動させる
/// 単体テスト用
/// </summary>
template <class... TArgs>
static void StartupEngineByUnitTest(std::function<HE::Bool()> in_runFunc,
                                       std::function<void()> in_endFunc = NULL)
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
    while (TRUE)
    {
        HE::Float32 d = HE_ENGINE.GetDeltaTimeSec();

        HE_ENGINE.BeforeUpdateLoop(d);
        if (HE_ENGINE.IsQuit()) break;

        d = HE_ENGINE.GetDeltaTimeSec();

        HE_ENGINE.WaitFrameLoop();
        if (HE_ENGINE.IsQuit()) break;

        HE_ENGINE.MainUpdateLoop(d);
        if (HE_ENGINE.IsQuit()) break;

        HE_ENGINE.LateUpdateLoop(d);
        if (HE_ENGINE.IsQuit()) break;

        // モジュールのテスト
        if (in_runFunc()) break;
    }

    if (in_endFunc)
    {
        in_endFunc();
    }

    // エンジン終了
    HE_DELETE_ENGINE;
}
#
