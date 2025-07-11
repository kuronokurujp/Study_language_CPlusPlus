#pragma once

#include "Engine/Common/PoolManager.h"
#include "Engine/Math/Math.h"
#include "Engine/MiniEngine.h"
#include "Engine/Platform/Graphic/Drawable.h"

namespace Platform
{
    struct SceneConfig;
    class FontInterface;
}  // namespace Platform

/// <summary>
/// 座標系は右手座標系
/// 2D/3Dの原点は画面の中心
/// </summary>
namespace PlatformSDL2
{
    // 前方宣言
    class ParticleMesh;

    /// <summary>
    /// 2Dの描画
    /// </summary>
    class Drawable2D : public Platform::Drawable2DInterface
    {
        HE_GENERATED_CLASS_BODY_HEADER(Drawable2D, Platform::Drawable2DInterface);

    public:
        Drawable2D();

        void VBegin() override final;
        void VEnd() override final;

        void VPreDraw() override final;
        void VPostDraw() override final;

        void VSetViewSize(const HE::Float32 in_fW, const HE::Float32 in_fH) override final;

        void SetFont(Core::Memory::SharedPtr<Platform::FontInterface>);

        /// <summary>
        /// 指定された数のパーティクルを作成
        /// 事前に生成しておく必要がある
        /// </summary>
        /// <param name="in_uCount">作成するパーティクルの数</param>
        /// <returns>作成されたパーティクルを識別するハンドル</returns>
        Core::Common::Handle CreateParticleObject(const HE::Uint32 in_uCount);

        /// <summary>
        /// 生成したパーティクルを削除
        /// </summary>
        void DeleteParticalObject(Core::Common::Handle);

        void SetArrayPosParticleObject(const Core::Common::Handle,
                                       const Core::Common::ArrayBase<Core::Math::Vector3>&);
        void SetArrtyVelocityParticelObject(const Core::Common::Handle,
                                            const Core::Common::ArrayBase<Core::Math::Vector3>&);

        /// <summary>
        /// 画面を色クリア
        /// </summary>
        void Cls(const HE::Uint32 in_uR, const HE::Uint32 in_uG, const HE::Uint32 in_uB);

        // パーティクルの描画
        void Draw2DPartical(const Core::Common::Handle, const Core::Math::Vector3&);

        /// <summary>
        /// 2Dのテキスト描画
        /// </summary>
        void Draw2DText(const Core::Math::Vector2& in_rPos,
                        const Core::Math::EAnchor in_eAnchor, const HE::Char* in_szText,
                        const HE::Uint32 in_uTextSize, const HE::Uint32 in_uSpace,
                        const Core::Math::Color);

        /// <summary>
        /// 2Dの矩形描画
        /// 座標は右手座標系で原点は画面の中心
        /// 矩形のアンカーは中央を基準にしている
        /// パラメータで矩形のアンカーを変える事が出来る
        /// </summary>
        void Draw2DQuad(const Core::Math::RC::Rect2D& in_rRect2D, Core::Math::EAnchor in_eAnchor,
                        const Core::Math::Color);

        /// <summary>
        /// 2Dの円描画
        /// </summary>
        void Draw2DCircle(const Core::Math::Vector2& in_rPos, const Core::Math::EAnchor in_eAchor,
                          const HE::Float32 in_fSize, const Core::Math::Color);

        /// <summary>
        /// 2Dの三角形描画
        /// </summary>
        void Draw2DTriangle(const Core::Math::Vector2& in_rPos, const Core::Math::EAnchor in_eAchor,
                            const HE::Float32 in_rAngleDegress, const HE::Float32 in_fSize,
                            const Core::Math::Color);

    private:
        using PoolParticleMesh              = Core::Common::RuntimePoolManager<ParticleMesh>;
        using CatchParticleMeshHandleVector = Core::Common::FixedVector<Core::Common::Handle, 256>;

    private:
        void* _pFontMesh = NULL;

        void* _p2DQuadMesh     = NULL;
        void* _p2DCircleMesh   = NULL;
        void* _p2DTriangleMesh = NULL;

        void* _p2DGeometoryMat = NULL;
        void* _p2DQuadMat      = NULL;

        void* _pWhiteTex = NULL;

        // パーティクルのマテリアルとメッシュプール
        void* _pParticleMat = NULL;
        PoolParticleMesh _poolParticleMesh;
        CatchParticleMeshHandleVector _vCatchParticleMeshHandle;

        Core::Math::Vector2 _viewSize;
        Core::Memory::SharedPtr<Platform::FontInterface> _spFont = NULL;

        HE::Bool _bBegin = FALSE;
    };
}  // namespace PlatformSDL2
