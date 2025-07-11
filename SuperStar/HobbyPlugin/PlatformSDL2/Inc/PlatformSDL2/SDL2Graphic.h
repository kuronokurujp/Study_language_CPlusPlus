#pragma once

// プラットフォームのスクリーンインターフェイス
// スクリーン情報取得や設定などができる

#include <functional>

#include "Engine/Math/Math.h"
#include "Engine/MiniEngine.h"
#include "Engine/Platform/PlatformFont.h"
#include "Engine/Platform/PlatformGraphic.h"
#include "Engine/Platform/PlatformInput.h"

namespace PlatformSDL2
{
    /// <summary>
    /// SDL2のグラフィックモジュール
    /// </summary>
    class Graphic final : public Platform::GraphicInterface
    {
    public:
        using FontInterfaceGetterFunc   = std::function<Platform::FontInterface*()>;
        using EventInputInterfaceGetter = std::function<Platform::InputInterface*()>;

    public:
        Graphic(FontInterfaceGetterFunc, EventInputInterfaceGetter);
        void VRelease() override final;

        void VUpdate(const HE::Float32) override final;

        HE::Bool IsMainWindowActive() const { return this->_bMainFrameActive; }

        /// ウィンドウロジックを生成
        /// プラットフォーム固有のウィンドウ生成などをする
        /// </summary>
        Core::Common::Handle VCreateFrame(
            const Platform::FrameConfig&,
            Core::Memory::UniquePtr<Platform::EventFrameInterface>) override final;

        virtual Platform::FrameBase* VGetFrame(const Core::Common::Handle) override final;

        /// <summary>
        /// 描画可能なオブジェクトを追加
        /// 継承したクラスで追加する
        /// 同じオブジェクトを何度も追加することはできない
        /// </summary>
        void VAddDrawable(
            Core::Memory::UniquePtr<Platform::Drawable2DInterface>) override final;

        /// <summary>
        /// 描画
        /// </summary>
        void VDraw() override final;

    private:
        using MapFrame = Core::Common::FixedMap<Core::Common::Handle,
                                                Core::Memory::UniquePtr<Platform::FrameBase>, 32>;

    private:
        FontInterfaceGetterFunc _fontInterfaceGetterFunc    = NULL;
        EventInputInterfaceGetter _inputInterfaceGetterFunc = NULL;

        HE::Bool _bMainFrameActive = FALSE;

        MapFrame _mFrame;
        MapFrame _mPendingFrame;
        HE::Uint32 _uFrameCount = 0;

        Platform::MapDrawable _mDrawable;
        Platform::MapDrawable _mPedingDrawable;
    };

}  // namespace PlatformSDL2
