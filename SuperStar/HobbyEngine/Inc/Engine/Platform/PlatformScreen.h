#pragma once

#include <functional>

#include "Engine/Common/CustomMap.h"
#include "Engine/Common/Handle.h"
#include "Engine/Math/Math.h"
#include "Engine/MiniEngine.h"

// スクリーン関連
#include "Engine/Platform/Screen/Scene.h"
#include "Engine/Platform/Screen/ViewPort.h"
#include "Engine/Platform/Screen/Window.h"

// プラットフォームの描画に関わる機能

namespace Platform
{
    /// <summary>
    /// スクリーンのインターフェイス
    /// </summary>
    class ScreenInterface
    {
    public:
        virtual ~ScreenInterface() = default;
        virtual void VRelease()    = 0;

        virtual void VUpdate(const HE::Float32)                                  = 0;

        virtual Core::Common::Handle VCreateWindowStrategy(const WindowConfig&) = 0;
        virtual WindowStrategy* VGetWindow(const Core::Common::Handle)          = 0;

        virtual void VRender() = 0;
    };

#if 0
    /// <summary>
    /// レンダリングのインターフェイス
    /// </summary>
    class RenderInterface
    {
    public:
        /// <summary>
        /// 指定された数のパーティクルを作成
        /// 事前に生成しておく必要がある
        /// </summary>
        /// <param name="in_uCount">作成するパーティクルの数</param>
        /// <returns>作成されたパーティクルを識別するハンドル</returns>
        virtual Core::Common::Handle VParticalCreate(const HE::Uint32 in_uCount) = 0;

        /// <summary>
        /// 生成したパーティクルを削除
        /// </summary>
        virtual void VParticalDelete(Core::Common::Handle) = 0;

        virtual void VParticalSetPositions(const Core::Common::Handle,
                                           const Core::Common::ArrayBase<Core::Math::Vector3>&) = 0;
        virtual void VParticalSetVelocitys(const Core::Common::Handle,
                                           const Core::Common::ArrayBase<Core::Math::Vector3>&) = 0;

        /// <summary>
        /// 画面を色クリア
        /// </summary>
        virtual void VCls(const HE::Uint32 in_uR, const HE::Uint32 in_uG,
                          const HE::Uint32 in_uB) = 0;

        // パーティクルの描画
        virtual void V2DDrawPartical(const Platform::ViewPortConfig& in_rViewConfig,
                                     const Core::Common::Handle in_rParticleHandle,
                                     const Core::Math::Vector3&) = 0;

        /// <summary>
        /// 2Dのテキスト描画
        /// </summary>
        virtual void V2DDrawText(const ViewPortConfig&, const Core::Math::Vector2& in_rPos,
                                 const Core::Math::EAnchor in_eAnchor, const HE::Char* in_szText,
                                 const HE::Uint32 in_uTextSize, const HE::Uint32 in_uSpace,
                                 const Core::Math::Color) = 0;

        /// <summary>
        /// 2Dの矩形描画
        /// </summary>
        virtual void V2DDrawQuad(const ViewPortConfig& in_rViewConfig,
                                 const Core::Math::CenteredRect2D& in_rRect2D, const Core::Math::Color) = 0;

        /// <summary>
        /// 2Dの円描画
        /// </summary>
        virtual void V2DDrawCircle(const ViewPortConfig& in_rViewConfig,
                                   const Core::Math::Vector2& in_rPos,
                                   const Core::Math::EAnchor in_eAchor, const HE::Float32 in_fSize,
                                   const Core::Math::Color) = 0;

        /// <summary>
        /// 2Dの三角形描画
        /// </summary>
        virtual void V2DDrawTriangle(const ViewPortConfig& in_rViewConfig,
                                     const Core::Math::Vector2& in_rPos,
                                     const Core::Math::EAnchor in_eAchor,
                                     const HE::Float32 in_rAngleDegress, const HE::Float32 in_fSize,
                                     const Core::Math::Color) = 0;
    };
#endif
}  // namespace Platform
