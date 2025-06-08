#pragma once

// プラットフォームのスクリーンインターフェイス
// スクリーン情報取得や設定などができる

#include "Engine/Math/Math.h"
#include "Engine/MiniEngine.h"
#include "Engine/Platform/PlatformScreen.h"

namespace PlatformSDL2
{
    // 前方宣言
    class PlatformSDL2Module;

    class Screen final : public Platform::SceneInterface, public Platform::RenderInterface
    {
    public:
        // このクラスはModule下にあるのでこのクラスが生きている間はModuleは必ず存在しているのを保障している
        Screen(PlatformSDL2::PlatformSDL2Module*);
        void VRelease() override final;

        /// <summary>
        /// スクリーン描画インターフェイス
        /// </summary>
        RenderInterface* VGetRenderer() override final;

        /// <summary>
        /// ウィンドウロジックを生成
        /// プラットフォーム固有のウィンドウ生成などをする
        /// </summary>
        Core::Memory::UniquePtr<Platform::WindowStrategy> VCreateWindowStrategy(
            const Core::Common::Handle, const Platform::WindowConfig&) override final;

        /// <summary>
        /// ビューポートロジックを生成
        /// プラットフォーム固有の処理をする
        /// </summary>
        Core::Memory::UniquePtr<Platform::ViewPortStrategy> VCreateViewPortStrategy(
            const Platform::ViewPortConfig&) override final;

        /// <summary>
        /// UIのシーンを生成
        /// </summary>
        Core::Memory::UniquePtr<Platform::SceneStrategyInterface> VCreateSceneUIStrategy()
            override final;

        /// <summary>
        /// 2Dのシーンを生成
        /// </summary>
        Core::Memory::UniquePtr<Platform::SceneStrategyInterface> VCreateScene2DStrategy()
            override final;

        // パーティクルの生成
        // 描画は戻り値のハンドルを指定
        Core::Common::Handle VParticalCreate(const HE::Uint32 in_uCount) override final;

        /// <summary>
        /// 生成したパーティクルを削除
        /// </summary>
        void VParticalDelete(Core::Common::Handle) override final;

        void VParticalSetPositions(
            const Core::Common::Handle,
            const Core::Common::ArrayBase<Core::Math::Vector3>&) override final;
        void VParticalSetVelocitys(
            const Core::Common::Handle,
            const Core::Common::ArrayBase<Core::Math::Vector3>&) override final;

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

    private:
        PlatformSDL2::PlatformSDL2Module* _pSDL2Module = NULL;
        void* _pFontMesh                               = NULL;

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
