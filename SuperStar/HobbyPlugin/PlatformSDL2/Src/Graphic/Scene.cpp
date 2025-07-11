#include "PlatformSDL2/Screen/Scene.h"

#include "PlatformSDL2/Screen/Draw/Material.h"
#include "PlatformSDL2/Screen/Draw/Mesh.h"

// SDL2関連
#include "GL/glew.h"
#include "SDL2/SDL.h"

// エンジン関連
#include "Engine/Engine.h"
#include "Engine/Math/Math.h"
#include "Engine/Platform/PlatformModule.h"

namespace PlatformSDL2
{
    SDL2SceneStrategy2D::SDL2SceneStrategy2D(
        const Platform::SceneConfig& in_rConfig,
        Core::Memory::SharedPtr<Platform::RenderInterface> in_spRenderInterface,
        EventRender in_eventRender)
        : Platform::SceneStrategy(in_rConfig, in_spRenderInterface, std::move(in_eventRender))
    {
    }

    HE::Bool SDL2SceneStrategy2D::VBegin()
    {
        return TRUE;
    }

    void SDL2SceneStrategy2D::VEnd()
    {
    }

    void SDL2SceneStrategy2D::VRender()
    {
        // 描画のブレンド設定
        ::glDisable(GL_DEPTH_TEST);
        ::glEnable(GL_BLEND);

        // アルファブレンド設定
        // srcFactor * srcAlpha + (1 - srcAlpha) * dstFactor
        ::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // ユーザーの描画処理を呼び出す
        HE_ASSERT(this->_eventRender);
        HE_ASSERT(this->_spRender);
        this->_eventRender(this->_spRender, this->_config);
    }
}  // namespace PlatformSDL2
