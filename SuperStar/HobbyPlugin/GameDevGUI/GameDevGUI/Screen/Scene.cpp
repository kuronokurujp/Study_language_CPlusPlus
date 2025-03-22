#include "Scene.h"

#include "imgui.h"

#ifdef HE_USE_SDL2

#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"

#endif

namespace GameDevGUI
{
    GameDevGUISceneStrategy::GameDevGUISceneStrategy(
        Core::Memory::UniquePtr<Platform::SceneStrategyInterface> in_upSt)
    {
        this->_upSt = std::move(in_upSt);
    }

    HE::Bool GameDevGUISceneStrategy::VBegin()
    {
        auto bRet = this->_upSt->VBegin();

        return bRet;
    }

    void GameDevGUISceneStrategy::VEnd()
    {
        this->_upSt->VEnd();
        HE_SAFE_DELETE_UNIQUE_PTR(this->_upSt);
    }

    void GameDevGUISceneStrategy::VUpdate(const HE::Float32 in_fDt)
    {
        this->_upSt->VUpdate(in_fDt);
    }

    void GameDevGUISceneStrategy::VBeginRender()
    {
        this->_upSt->VBeginRender();

        ::ImGui::Begin("Window");
        // TODO: GUI処理を記述する
        if (::ImGui::Button("test"))
        {
            HE_LOG_LINE(HE_STR_TEXT("Push"));
        }
    }

    void GameDevGUISceneStrategy::VEndRender()
    {
        ::ImGui::End();
        this->_upSt->VEndRender();
    }
}  // namespace GameDevGUI
