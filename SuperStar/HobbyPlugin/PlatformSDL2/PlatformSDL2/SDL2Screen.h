#pragma once

// プラットフォームのスクリーンインターフェイス
// スクリーン情報取得や設定などができる

#include "Engine/Common/CustomVector.h"
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
        /// TODO: 画面を色クリア
        /// </summary>
        void VCls(const HE::Uint32 in_uR, const HE::Uint32 in_uG,
                  const HE::Uint32 in_uB) override final;

        /// <summary>
        /// TODO: 2Dテキスト描画
        /// </summary>
        void VDrawText2D(const Core::Math::Vector2& in_rPos, const HE::Char* in_szText,
                         const Core::Math::Rect2::EAnchor in_eAnchor,
                         const Core::Math::Color& in_rColor) override final;

    private:
        PlatformSDL2::PlatformSDL2Module* _pSDL2Module = NULL;
        void* _pFontMesh                               = NULL;
    };

}  // namespace PlatformSDL2
