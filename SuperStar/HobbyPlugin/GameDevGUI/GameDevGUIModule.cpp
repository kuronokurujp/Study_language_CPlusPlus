#include "GameDevGUIModule.h"

#include "Engine/Engine.h"
#include "Engine/Platform/PlatformModule.h"
#include "imgui.h"

#ifdef HE_USE_SDL2

#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"

#endif

namespace GameDevGUI
{
    void GameDevGUIModule::CreateFrame(Platform::WindowStrategy* in_pSt)
    {
        // TODO: ウィンドウとコンテキストを取得
#ifdef HE_USE_SDL2
        SDL_Window* pWindow = reinterpret_cast<SDL_Window*>(in_pSt->GetWindowBySDL2());
        void* pContext      = in_pSt->GetContentBySDL2();

        IMGUI_CHECKVERSION();

        auto pImgGuiContext = ::ImGui::CreateContext();
        ImGuiIO& io         = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui::StyleColorsDark();

        ::ImGui_ImplSDL2_InitForOpenGL(pWindow, pContext);

        auto pPlatformModule = HE_ENGINE.PlatformModule();
        ::ImGui_ImplOpenGL3_Init(pPlatformModule->GetOpenGLVersionNameBySDL2());

        // ウィンドウの描画開始と終了のコールバック
        in_pSt->AddBeginRenderCallback([this]() { ImGui::Render(); });
        in_pSt->AddEndRenderCallback([this]()
                                     { ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); });
        in_pSt->AddUpdateRenderCallback(
            [pImgGuiContext](const HE::Float32 in_fDt)
            {
                // ImGuiのコンテキストを変更
                ::ImGui::SetCurrentContext(pImgGuiContext);

                ::ImGui_ImplOpenGL3_NewFrame();
                ::ImGui_ImplSDL2_NewFrame();

                ::ImGui::NewFrame();
            });
        this->_mImGui.Add(in_pSt->GetHandle(), pImgGuiContext);
#endif
    }

    void GameDevGUIModule::DestoryFrame(const Platform::WindowStrategy* in_pSt)
    {
        // TODO: 作成したフレームを破棄
#ifdef HE_USE_SDL2
        ::ImGuiContext* pImgGuiContext =
            reinterpret_cast<::ImGuiContext*>(this->_mImGui.FindKey(in_pSt->GetHandle())->_data);
        if (pImgGuiContext == NULL) return;

        ::ImGui_ImplOpenGL3_Shutdown();
        ::ImGui_ImplSDL2_Shutdown();
        ::ImGui::DestroyContext(pImgGuiContext);

        this->_mImGui.Erase(in_pSt->GetHandle());
#endif
    }

    /// <summary>
    /// モジュール初期化
    /// </summary>
    HE::Bool GameDevGUIModule::_VStart()
    {
#ifdef HE_USE_SDL2
        this->_mImGui.Clear();
        auto pPlatformModule = HE_ENGINE.PlatformModule();
        pPlatformModule->VInput()->SetInputEventCallback(
            [this](void* in_pEvent)
            {
                SDL_Event* pEvent = reinterpret_cast<SDL_Event*>(in_pEvent);
                for (auto it = this->_mImGui.Begin(); it != this->_mImGui.End(); ++it)
                {
                    ImGui_ImplSDL2_ProcessEvent(pEvent);
                }
            });
#endif
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
