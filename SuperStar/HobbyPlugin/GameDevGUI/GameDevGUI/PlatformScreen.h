#pragma once

// プラットフォームのスクリーンインターフェイス
// スクリーン情報取得や設定などができる

#include "Engine/Math/Math.h"
#include "Engine/MiniEngine.h"
#include "Engine/Platform/PlatformScreen.h"

namespace GameDevGUI
{
    /// <summary>
    /// DevGUIのシーンレンダリング
    /// </summary>
    class ScreenRender final : public Platform::RenderInterface
    {
    public:
        /// <summary>
        /// 画面を色クリア
        /// </summary>
        void VCls(const HE::Uint32 in_uR, const HE::Uint32 in_uG,
                  const HE::Uint32 in_uB) override final;

        // 2D上でのパーティクル描画
        void V2DDrawPartical(const Platform::ViewPortConfig& in_rViewConfig,
                             const Core::Common::Handle in_rParticleHandle,
                             const Core::Math::Vector3&) override final;

        /// <summary>
        /// 2Dテキスト描画
        /// </summary>
        void V2DDrawText(const Platform::ViewPortConfig& in_rViewConfig,
                         const Core::Math::Vector2& in_rPos, const Core::Math::EAnchor in_eAnchor,
                         const HE::Char* in_szText, const HE::Uint32 in_uTextSize,
                         const HE::Uint32 in_uSpace,
                         const Core::Math::Color in_color) override final;

        /// <summary>
        /// 2Dの矩形描画
        /// </summary>
        void V2DDrawQuad(const Platform::ViewPortConfig& in_rViewConfig,
                         const Core::Math::Rect2& in_rRect2D,
                         const Core::Math::Color in_color) override final;

        /// <summary>
        /// 2Dの円描画
        /// </summary>
        void V2DDrawCircle(const Platform::ViewPortConfig& in_rViewConfig,
                           const Core::Math::Vector2& in_rPos, const Core::Math::EAnchor in_eAchor,
                           const HE::Float32 in_fSize,
                           const Core::Math::Color in_color) override final;

        /// <summary>
        /// 2Dの三角形描画
        /// </summary>
        void V2DDrawTriangle(const Platform::ViewPortConfig& in_rViewConfig,
                             const Core::Math::Vector2& in_rPos,
                             const Core::Math::EAnchor in_eAchor,
                             const HE::Float32 in_rAngleDegress, const HE::Float32 in_fSize,
                             const Core::Math::Color) override final;
    };

}  // namespace GameDevGUI
