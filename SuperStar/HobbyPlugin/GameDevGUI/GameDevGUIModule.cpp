#include "GameDevGUIModule.h"

#include "./GameDevGUI/Screen/Scene.h"
#include "./GameDevGUI/Screen/Window.h"
#include "Engine/Engine.h"
#include "Engine/Platform/PlatformModule.h"
#include "imgui.h"

#ifdef HE_USE_SDL2

#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"

#endif

#include "RenderModule.h"

namespace GameDevGUI
{
    GameDevGUIModule::GameDevGUIModule() : ModuleBase(ModuleName())
    {
        // 依存モジュール
        this->_AppendDependenceModule<Render::RenderModule>();
    }

    Core::Memory::UniquePtr<Platform::WindowStrategy> GameDevGUIModule::CreateWindowStrategy(
        const Core::Common::Handle in_handle, const Platform::WindowConfig& in_rConfig)
    {
        // ImGUI用のを作成
        auto pPlatformModule = HE_ENGINE.PlatformModule();

        auto upSt = HE_MAKE_CUSTOM_UNIQUE_PTR((GameDevGUIWindowStrategy),
                                              pPlatformModule->VScreen()
                                                  ->VCreateWindowStrategy(in_handle, in_rConfig));

        return std::move(upSt);
    }

    Core::Memory::UniquePtr<GameDevGUISceneStrategy> GameDevGUIModule::CreateSceneStrategy()
    {
        auto pPlatformModule = HE_ENGINE.PlatformModule();

        // ImGUI用のプラットフォームのスクリーンを作成
        auto upSt = HE_MAKE_CUSTOM_UNIQUE_PTR((GameDevGUISceneStrategy),
                                              pPlatformModule->VScreen()->VCreateSceneUIStrategy());
        return std::move(upSt);
    }

    /// <summary>
    /// モジュール初期化
    /// </summary>
    HE::Bool GameDevGUIModule::_VStart()
    {
        return TRUE;
    }

    /// <summary>
    /// インスタンス破棄時に呼ばれる
    /// </summary>
    HE::Bool GameDevGUIModule::_VRelease()
    {
        return TRUE;
    }

    void GameDevGUIModule::_VUpdate(const HE::Float32 in_fDeltaTime)
    {
    }

}  // namespace GameDevGUI
