#pragma once

#include "Engine/Common/Handle.h"
#include "Engine/Core.h"

// プラットフォームのウィンドウやスクリーンを構築
// スクリーン情報取得や設定などができる
// ウィンドウの生成 / ビューポート / 描画シーンなどを取り扱う

namespace Platform
{
    struct ScreenSceneViewUIConfig
    {
    public:
        ScreenSceneViewUIConfig(const Core::Common::Handle& in_windowHandle,
                                const Core::Common::Handle& in_viewPortHandle);

        Core::Common::Handle _windowHandle;
        Core::Common::Handle _viewPortHandle;
    };

    struct ScreenSceneView2DConfig
    {
    public:
        ScreenSceneView2DConfig(const Core::Common::Handle& in_windowHandle,
                                const Core::Common::Handle& in_viewPortHandle);

        Core::Common::Handle _windowHandle;
        Core::Common::Handle _viewPortHandle;
    };

    // TODO: 2Dシーンの環境
    struct ScreenSceneView2DEnvironment
    {
    public:
        HE::Uint32 _uWidth  = 0;
        HE::Uint32 _uHeight = 0;
    };

    class ScreenInterface
    {
    public:
        virtual ~ScreenInterface() = default;

        virtual const Core::Common::Handle VCreateWindow() = 0;
        virtual HE::Bool VReleaseAllWindows()              = 0;

        /// <summary>
        /// ウィンドウ描画
        /// </summary>
        virtual void VShowWindow(const Core::Common::Handle&) = 0;

        virtual Core::Common::Handle VAddViewPort(const Core::Common::Handle&)       = 0;
        virtual void VRemoveViewPort(const Core::Common::Handle& in_rWindowHandle,
                                     const Core::Common::Handle& in_rViewPortHandle) = 0;

        /// <summary>
        /// UI用シーン追加
        /// </summary>
        virtual const Core::Common::Handle& VAddSceneViewUI(
            const ScreenSceneViewUIConfig& in_rConfig) = 0;

        /// <summary>
        /// 2D用シーン追加
        /// </summary>
        virtual const Core::Common::Handle& VAddSceneView2D(
            const ScreenSceneView2DConfig& in_rConfig) = 0;

        /// <summary>
        /// 2D環境取得
        /// </summary>
        virtual const ScreenSceneView2DEnvironment&& GetEnvBySceneView2D(
            const Core::Common::Handle&) = 0;
    };

}  // namespace Platform
