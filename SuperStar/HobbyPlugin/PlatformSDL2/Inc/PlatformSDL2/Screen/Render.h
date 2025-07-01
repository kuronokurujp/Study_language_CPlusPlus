#pragma once

#include "Engine/Math/Math.h"
#include "Engine/MiniEngine.h"
#include "Engine/Platform/Screen/Render.h"

namespace Platform
{
    struct SceneConfig;
    class FontInterface;
}  // namespace Platform

namespace PlatformSDL2
{
    /// <summary>
    /// レンダリングのインターフェイス
    /// </summary>
    class DefaultRender : public Platform::RenderInterface
    {
        HE_GENERATED_CLASS_BODY_HEADER(DefaultRender, Platform::RenderInterface);

    public:
        DefaultRender();

        void VBegin() override final;
        void VEnd() override final;

        /// <summary>
        /// 指定された数のパーティクルを作成
        /// 事前に生成しておく必要がある
        /// </summary>
        /// <param name="in_uCount">作成するパーティクルの数</param>
        /// <returns>作成されたパーティクルを識別するハンドル</returns>
        Core::Common::Handle ParticalCreate(const HE::Uint32 in_uCount);

        /// <summary>
        /// 生成したパーティクルを削除
        /// </summary>
        void ParticalDelete(Core::Common::Handle);

        void ParticalSetPositions(const Core::Common::Handle,
                                  const Core::Common::ArrayBase<Core::Math::Vector3>&);
        void ParticalSetVelocitys(const Core::Common::Handle,
                                  const Core::Common::ArrayBase<Core::Math::Vector3>&);

        /// <summary>
        /// 画面を色クリア
        /// </summary>
        void Cls(const HE::Uint32 in_uR, const HE::Uint32 in_uG, const HE::Uint32 in_uB);

        // パーティクルの描画
        void Draw2DPartical(const Platform::SceneConfig& in_rViewConfig,
                            const Core::Common::Handle in_rParticleHandle,
                            const Core::Math::Vector3&);

        /// <summary>
        /// 2Dのテキスト描画
        /// </summary>
        void Draw2DText(const Platform::SceneConfig&, Platform::FontInterface* in_pFont,
                        const Core::Math::Vector2& in_rPos, const Core::Math::EAnchor in_eAnchor,
                        const HE::Char* in_szText, const HE::Uint32 in_uTextSize,
                        const HE::Uint32 in_uSpace, const Core::Math::Color);

        /// <summary>
        /// 2Dの矩形描画
        /// </summary>
        void Draw2DQuad(const Platform::SceneConfig& in_rViewConfig,
                        const Core::Math::Rect2& in_rRect2D, const Core::Math::Color);

        /// <summary>
        /// 2Dの円描画
        /// </summary>
        void Draw2DCircle(const Platform::SceneConfig& in_rViewConfig,
                          const Core::Math::Vector2& in_rPos, const Core::Math::EAnchor in_eAchor,
                          const HE::Float32 in_fSize, const Core::Math::Color);

        /// <summary>
        /// 2Dの三角形描画
        /// </summary>
        void Draw2DTriangle(const Platform::SceneConfig& in_rViewConfig,
                            const Core::Math::Vector2& in_rPos, const Core::Math::EAnchor in_eAchor,
                            const HE::Float32 in_rAngleDegress, const HE::Float32 in_fSize,
                            const Core::Math::Color);

    private:
        void* _pFontMesh = NULL;

        void* _p2DQuadMesh     = NULL;
        void* _p2DCircleMesh   = NULL;
        void* _p2DTriangleMesh = NULL;

        void* _p2DGeometoryMat = NULL;
        void* _p2DQuadMat      = NULL;

        void* _pWhiteTex = NULL;

        // パーティクルのマテリアルとメッシュプール
        void* _pParticleMat      = NULL;
        void* _pPoolParticleMesh = NULL;
    };
}  // namespace PlatformSDL2
