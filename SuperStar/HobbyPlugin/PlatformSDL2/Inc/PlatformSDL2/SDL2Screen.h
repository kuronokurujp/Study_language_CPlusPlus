#pragma once

// プラットフォームのスクリーンインターフェイス
// スクリーン情報取得や設定などができる

#include <functional>

#include "Engine/Math/Math.h"
#include "Engine/MiniEngine.h"
#include "Engine/Platform/PlatformFont.h"
#include "Engine/Platform/PlatformInput.h"
#include "Engine/Platform/PlatformScreen.h"

namespace PlatformSDL2
{
    class Screen final : public Platform::ScreenInterface
    {
    public:
        using FontInterfaceGetterFunc  = std::function<Platform::FontInterface*()>;
        using EventInputInterfaceGetter = std::function<Platform::InputInterface*()>;

    public:
        Screen(FontInterfaceGetterFunc,
               EventInputInterfaceGetter);
        void VRelease() override final;

        HE::Bool IsMainWindowActive() const { return this->_bMainWindowActive; }

        /// <summary>
        /// スクリーン描画インターフェイス
        /// </summary>
        //RenderInterface* VGetRenderer() override final;

        /// <summary>
        /// ウィンドウロジックを生成
        /// プラットフォーム固有のウィンドウ生成などをする
        /// </summary>
        Core::Common::Handle VCreateWindowStrategy(const Platform::WindowConfig&) override final;

        virtual Platform::WindowStrategy* VGetWindow(const Core::Common::Handle) override final;

        virtual void VRendering() override final;

    private:
        using MapWindowSt =
            Core::Common::FixedMap<Core::Common::Handle,
                                   Core::Memory::UniquePtr<Platform::WindowStrategy>, 32>;

    private:
        FontInterfaceGetterFunc _fontInterfaceGetterFunc   = NULL;
        EventInputInterfaceGetter _inputInterfaceGetterFunc = NULL;

        HE::Bool _bMainWindowActive = FALSE;

        MapWindowSt _mWindowStrategy;
        HE::Uint32 _uWindowStCount = 0;
    };

}  // namespace PlatformSDL2
