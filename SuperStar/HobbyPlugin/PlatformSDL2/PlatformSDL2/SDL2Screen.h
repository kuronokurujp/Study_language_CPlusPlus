#pragma once

// プラットフォームのスクリーンインターフェイス
// スクリーン情報取得や設定などができる

#include "Engine/Common/CustomVector.h"
#include "Engine/MiniEngine.h"
#include "Engine/Platform/PlatformScreen.h"

namespace PlatformSDL2
{
    class PlatformSDL2Module;
    class Screen final : public Platform::ScreenInterface
    {
    public:
        // このクラスはDXLibModule下にあるのでこのクラスが生きている間はDXLibModuleは必ず存在しているのを保障している
        Screen(PlatformSDL2::PlatformSDL2Module*);

        const Core::Common::Handle VCreateWindow() override final;
        HE::Bool VReleaseAllWindows() override final;

        void VShowWindow(const Core::Common::Handle&) override final;

        Core::Common::Handle VAddViewPort(const Core::Common::Handle&) override final;
        void VRemoveViewPort(const Core::Common::Handle& in_rWindowHandle,
                             const Core::Common::Handle& in_rViewPortHandle) override final;

        // TODO: UI用シーン追加
        const Core::Common::Handle& VAddSceneViewUI(
            const Platform::ScreenSceneViewUIConfig&) override final;

        // TODO: 2D用シーン追加
        const Core::Common::Handle& VAddSceneView2D(
            const Platform::ScreenSceneView2DConfig&) override final;

        // TODO: 2D環境取得
        const Platform::ScreenSceneView2DEnvironment&& GetEnvBySceneView2D(
            const Core::Common::Handle&) override final;

    private:
        Core::Common::FixedVector<Core::Common::Handle, 32> _vWindowHandle;

        PlatformSDL2::PlatformSDL2Module* _pSDL2Module = NULL;
    };

}  // namespace PlatformSDL2
