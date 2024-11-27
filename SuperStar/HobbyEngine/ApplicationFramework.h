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
    virtual Bool VStart(const Bool in_bDebug);

    /// <summary>
    /// ゲームループ
    /// </summary>
    virtual Bool VGameLoop();

    /// <summary>
    /// ゲーム終了
    /// </summary>
    virtual Bool VEnd();

protected:
    /// <summary>
    /// エンジンのモジュールを登録・解除
    /// </summary>
    virtual Bool _VRegistEngineModule()   = 0;
    virtual Bool _VUnRegistEngineModule() = 0;
};
