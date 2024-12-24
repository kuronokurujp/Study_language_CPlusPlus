#pragma once

// プラットフォームのスクリーンインターフェイス
// スクリーン情報取得や設定などができる

#include "Engine/MiniEngine.h"
#include "Engine/Platform/PlatformScreen.h"

namespace PlatformSDL2
{
    // 前方宣言
    class PlatformSDL2Module;

    class Screen final : public Platform::ScreenInterface
    {
    public:
        // このクラスはModule下にあるのでこのクラスが生きている間はModuleは必ず存在しているのを保障している
        Screen(PlatformSDL2::PlatformSDL2Module*);
        void VRelease() override final;

        Core::Memory::UniquePtr<Platform::WindowStrategy> VCreateWindowStrategy(
            const Platform::WindowConfig&) override final;

        Core::Memory::UniquePtr<Platform::ViewPortStrategy> VCreateViewPortStrategy(
            const Platform::ViewPortConfig&) override final;

        Core::Memory::UniquePtr<Platform::SceneStrategyInterface> VCreateSceneUIStrategy()
            override final;
        Core::Memory::UniquePtr<Platform::SceneStrategyInterface> VCreateScene2DStrategy()
            override final;

        /// <summary>
        /// 画面を色クリア
        /// </summary>
        void VCls(const HE::Uint32 in_uR, const HE::Uint32 in_uG,
                  const HE::Uint32 in_uB) override final;

        /// <summary>
        /// 2Dテキスト描画
        /// </summary>
        void VDrawText2D(const Platform::ViewPortConfig& in_rViewConfig,
                         const Core::Math::Vector2& in_rPos, const Core::Math::EAnchor in_eAnchor,
                         const HE::Char* in_szText, const HE::Uint32 in_uTextSize,
                         const Core::Math::Color in_color) override final;

        /// <summary>
        /// 2Dの矩形描画
        /// </summary>
        void VDrawQuad2D(const Platform::ViewPortConfig& in_rViewConfig,
                         const Core::Math::Rect2& in_rRect2D,
                         const Core::Math::Color in_color) override final;

        /// <summary>
        /// 2Dの円描画
        /// </summary>
        void VDrawCircle2D(const Platform::ViewPortConfig& in_rViewConfig,
                           const Core::Math::Vector2& in_rPos, const Core::Math::EAnchor in_eAchor,
                           const HE::Float32 in_fSize,
                           const Core::Math::Color in_color) override final;

    private:
        PlatformSDL2::PlatformSDL2Module* _pSDL2Module = NULL;
        void* _pFontMesh                               = NULL;

        void* _p2DQuadMesh   = NULL;
        void* _p2DCircleMesh = NULL;

        void* _p2DGeometoryMat = NULL;
        void* _p2DQuadMat      = NULL;

        void* _pWhiteTex = NULL;
    };

}  // namespace PlatformSDL2
