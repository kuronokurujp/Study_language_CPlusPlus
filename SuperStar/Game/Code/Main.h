#pragma once

#include "ApplicationFramework.h"

#ifdef HE_WIN

/// <summary>
/// アプリのゲームメインのエントリー
/// ゲームの起動から更新,終了のロジックを記述
/// </summary>
class WinGameMain final : public ApplicationEngineFramework
{
public:
    WinGameMain(HINSTANCE in_instance, HE::Int in_cmdShow);

protected:
    /// <summary>
    /// ゲーム開始前のロード
    /// </summary>
    HE::Bool _VLoad() override final;

    /// <summary>
    /// ゲーム開始
    /// </summary>
    HE::Bool _VStart() override final;

    /// <summary>
    /// ゲーム終了
    /// </summary>
    HE::Bool _VEnd() override final;

    /// <summary>
    /// エンジンのモジュールを登録・解除
    /// </summary>
    virtual HE::Bool _VRegistEngineModule();
    virtual HE::Bool _VUnRegistEngineModule();

private:
    static constexpr HE::Uint32 uMaxLoadStringLength = 100;

    // タイトル バーのテキスト
    WCHAR _szTitle[uMaxLoadStringLength] = {0};

    // メイン ウィンドウ クラス名
    WCHAR _szWindowClass[uMaxLoadStringLength] = {0};

    HINSTANCE _inst;
    HE::Int _cmdShow = 0;
};

#endif
