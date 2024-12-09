#pragma once

#include "Engine/MiniEngine.h"

/// <summary>
/// エンジンをアプリで扱うフレームワーク
/// </summary>
class ApplicationEngineFramework
{
public:
    /// <summary>
    /// ゲーム開始
    /// </summary>
    virtual HE::Bool VStart(const HE::Bool in_bDebug);

    /// <summary>
    /// ゲームループ
    /// </summary>
    virtual HE::Bool VGameLoop();

    /// <summary>
    /// ゲーム終了
    /// </summary>
    virtual HE::Bool VEnd();

protected:
    /// <summary>
    /// エンジンのモジュールを登録・解除
    /// </summary>
    virtual HE::Bool _VRegistEngineModule()   = 0;
    virtual HE::Bool _VUnRegistEngineModule() = 0;
};
