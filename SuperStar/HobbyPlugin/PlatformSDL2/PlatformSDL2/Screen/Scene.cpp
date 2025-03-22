#include "Scene.h"

#include "./Draw/Material.h"
#include "./Draw/Mesh.h"

// SDL2関連
#include "GL/glew.h"
#include "SDL2/SDL.h"

// エンジン関連
#include "Engine/Engine.h"
#include "Engine/Math/Math.h"
#include "Engine/Platform/PlatformModule.h"

namespace PlatformSDL2
{
    HE::Bool SDL2SceneStrategy2D::VBegin()
    {
        return TRUE;
    }

    void SDL2SceneStrategy2D::VEnd()
    {
    }

    void SDL2SceneStrategy2D::VUpdate(const HE::Float32)
    {
    }

    void SDL2SceneStrategy2D::VBeginRender()
    {
        // 描画のブレンド設定
        ::glDisable(GL_DEPTH_TEST);
        ::glEnable(GL_BLEND);

        // アルファブレンド設定
        // srcFactor * srcAlpha + (1 - srcAlpha) * dstFactor
        ::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void SDL2SceneStrategy2D::VEndRender()
    {
    }

    Platform::ScreenRenderInterface* SDL2SceneStrategy2D::VGetPlatformScreenDraw()
    {
        // TODO:
        // スクリーンのプラットフォームは動的に切り替えたいのでストラテジー側から取得できるように変える
        auto pPlatformModule = HE_ENGINE.PlatformModule();
        HE_ASSERT(pPlatformModule);

        return pPlatformModule->VScreen()->VGetDrawInterface();
    }
}  // namespace PlatformSDL2
