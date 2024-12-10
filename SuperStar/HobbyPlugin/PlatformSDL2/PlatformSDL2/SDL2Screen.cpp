#pragma once

#include "SDL2Screen.h"

#include <algorithm>

#include "SDL2/SDL.h"
#include "Screen/Scene.h"
#include "Screen/Window.h"

// 依存モジュール一覧
#include "PlatformSDL2Module.h"
#include "RenderModule.h"

namespace PlatformSDL2
{
    Screen::Screen(PlatformSDL2::PlatformSDL2Module* in_pSDL2Module)
    {
        HE_ASSERT(in_pSDL2Module);

        this->_pSDL2Module = in_pSDL2Module;
    }

    const Core::Common::Handle Screen::VCreateWindow()
    {
        auto pRenderModule = this->_pSDL2Module->GetDependenceModule<Render::RenderModule>();
        if (pRenderModule == NULL) return FALSE;

        auto upWindowStrategy = HE_MAKE_CUSTOM_UNIQUE_PTR((DXLibWindowStrategy));
        auto handle           = pRenderModule->NewWindow(std::move(upWindowStrategy));

        this->_vWindowHandle.PushBack(handle);

        return handle;
    }

    HE::Bool Screen::VReleaseAllWindows()
    {
        auto pRenderModule = this->_pSDL2Module->GetDependenceModule<Render::RenderModule>();
        HE_ASSERT_RETURN_VALUE(FALSE, pRenderModule);

        pRenderModule->DeleteAllWindow();

        return TRUE;
    }

    Core::Common::Handle Screen::VAddViewPort(const Core::Common::Handle& in_rWindowHandle)
    {
        auto pRenderModule = this->_pSDL2Module->GetDependenceModule<Render::RenderModule>();
        HE_ASSERT_RETURN_VALUE(NullHandle, pRenderModule);

        Core::Memory::UniquePtr<Render::ViewPortConfig> upConfig =
            HE_MAKE_CUSTOM_UNIQUE_PTR((Render::ViewPortConfig));
        // TODO: 用意できるシーン数(仮)
        upConfig->uSceneCount = 4;

        return pRenderModule->AddViewPort(in_rWindowHandle, std::move(upConfig));
    }

    void Screen::VRemoveViewPort(const Core::Common::Handle& in_rWindowHandle,
                                 const Core::Common::Handle& in_rViewPortHandle)
    {
        auto pRenderModule = this->_pSDL2Module->GetDependenceModule<Render::RenderModule>();
        HE_ASSERT_RETURN(pRenderModule);

        pRenderModule->RemoveViewPort(in_rWindowHandle, in_rViewPortHandle);
    }

    const Core::Common::Handle& Screen::VAddSceneViewUI(
        const Platform::ScreenSceneViewUIConfig& in_rConfig)
    {
        // プラットフォーム専用のUIシーンを追加
        auto pRenderModule = this->_pSDL2Module->GetDependenceModule<Render::RenderModule>();
        HE_ASSERT_RETURN_VALUE(NullHandle, pRenderModule);

        const Core::Common::Handle& handle =
            pRenderModule->AddSceneViewUI<SceneViewUI>(in_rConfig._windowHandle,
                                                       in_rConfig._viewPortHandle);
        return handle;
    }

    const Core::Common::Handle& Screen::VAddSceneView2D(
        const Platform::ScreenSceneView2DConfig& in_rConfig)
    {
        // プラットフォーム専用の2Dシーンを追加
        auto pRenderModule = this->_pSDL2Module->GetDependenceModule<Render::RenderModule>();
        HE_ASSERT_RETURN_VALUE(NullHandle, pRenderModule);

        return pRenderModule->AddSceneView2D<SceneView2D>(in_rConfig._windowHandle,
                                                          in_rConfig._viewPortHandle);
    }

    const Platform::ScreenSceneView2DEnvironment&& Screen::GetEnvBySceneView2D(
        const Core::Common::Handle& in_rHandle)
    {
        auto env = Platform::ScreenSceneView2DEnvironment();

        // プラットフォーム専用の2Dシーンを追加
        auto pRenderModule = this->_pSDL2Module->GetDependenceModule<Render::RenderModule>();
        HE_ASSERT_RETURN_VALUE(std::move(env), pRenderModule);

        // ビューポートの設定情報取得
        // 設定情報から環境情報を構築
        auto pViewPortConfig = pRenderModule->GetViewPortConfig(in_rHandle);

        env._uWidth  = pViewPortConfig->uWidth;
        env._uHeight = pViewPortConfig->uHeight;

        return std::move(env);
    }

    void Screen ::VShowWindow(const Core::Common::Handle& in_rHandle)
    {
        auto pRenderModule = this->_pSDL2Module->GetDependenceModule<Render::RenderModule>();
        HE_ASSERT_RETURN(pRenderModule);

        pRenderModule->ShowWindow(in_rHandle);
    }

}  // namespace PlatformSDL2
