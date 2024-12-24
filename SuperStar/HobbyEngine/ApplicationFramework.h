#pragma once

#include "Engine/MiniEngine.h"

/// <summary>
/// エンジンをアプリで扱うフレームワーク
/// </summary>
class ApplicationEngineFramework
{
public:
    /// <summary>
    /// 初期化
    /// </summary>
    HE::Bool Init(const HE::Bool in_bDebug);

    /// <summary>
    /// 解放
    /// </summary>
    void Release();

    /// <summary>
    /// 処理中
    /// </summary>
    void Running();

protected:
    /// <summary>
    /// ゲーム開始前のロード
    /// </summary>
    virtual HE::Bool _VLoad() = 0;

    /// <summary>
    /// ゲーム開始
    /// </summary>
    virtual HE::Bool _VStart() = 0;

    /// <summary>
    /// ゲーム終了
    /// </summary>
    virtual HE::Bool _VEnd() = 0;

    /// <summary>
    /// エンジンのモジュールを登録・解除
    /// </summary>
    virtual HE::Bool _VRegistEngineModule()   = 0;
    virtual HE::Bool _VUnRegistEngineModule() = 0;
};
