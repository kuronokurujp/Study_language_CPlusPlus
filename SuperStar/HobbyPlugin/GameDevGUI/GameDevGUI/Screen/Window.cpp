#include "Window.h"

#include "Engine/Engine.h"
#include "Engine/Platform/PlatformModule.h"
#include "imgui.h"

#ifdef HE_USE_SDL2

#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"

#endif

namespace GameDevGUI
{
    GameDevGUIWindowStrategy::GameDevGUIWindowStrategy(
        Core::Memory::UniquePtr<Platform::WindowStrategy> in_upSt)
        : Platform::WindowStrategy(in_upSt->VGetHandle(), in_upSt->VGetConfig())
    {
        this->_upSt = std::move(in_upSt);
    }

    void GameDevGUIWindowStrategy::VRelease()
    {
        this->_upSt->VRelease();
        HE_SAFE_DELETE_UNIQUE_PTR(this->_upSt);
    }

    void GameDevGUIWindowStrategy::VBegin()
    {
        this->_upSt->VBegin();

        IMGUI_CHECKVERSION();

        this->_upSt->VActive();

        auto pImGuiContext = ::ImGui::CreateContext();
        ImGuiIO& io        = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui::StyleColorsDark();

#ifdef HE_USE_SDL2

        SDL_Window* pWindow = reinterpret_cast<SDL_Window*>(this->_upSt->VGetWindowBySDL2());
        void* pContext      = this->_upSt->VGetContentBySDL2();

        ::ImGui_ImplSDL2_InitForOpenGL(pWindow, pContext);
        // TODO: OpenGL名取得はwindowクラスに持った方がいいかな?
        auto pPlatformModule = HE_ENGINE.PlatformModule();
        ::ImGui_ImplOpenGL3_Init(pPlatformModule->GetOpenGLVersionNameBySDL2());

        // 入力イベントを取得
        auto pInputObj =
            HE_ENGINE.PlatformModule()->VInput()->GetObj(this->_upSt->VGetConfig().InputHandle());
        pInputObj->SetEventCallback(
            [](void* in_pEvent)
            {
                const SDL_Event* pSDLEvent = reinterpret_cast<const SDL_Event*>(in_pEvent);
                HE_ASSERT(pSDLEvent);
                ::ImGui_ImplSDL2_ProcessEvent(pSDLEvent);
            });

#endif
        this->_pImGuiContext = pImGuiContext;
    }

    void GameDevGUIWindowStrategy::VEnd()
    {
#ifdef HE_USE_SDL2
        ::ImGui_ImplOpenGL3_Shutdown();
        ::ImGui_ImplSDL2_Shutdown();
#endif
        ::ImGuiContext* pImGuiContext = reinterpret_cast<::ImGuiContext*>(this->_pImGuiContext);
        ::ImGui::DestroyContext(pImGuiContext);

        this->_upSt->VEnd();

        this->_pImGuiContext = NULL;
    }

    void GameDevGUIWindowStrategy::VUpdate(const HE::Float32 in_dt)
    {
        this->_upSt->VUpdate(in_dt);
    }

    void GameDevGUIWindowStrategy::VSetPos(const HE::Uint32 in_uX, const HE::Uint32 in_uY)
    {
        this->_upSt->VSetPos(in_uX, in_uY);
    }

    void GameDevGUIWindowStrategy::VActive()
    {
        this->_upSt->VActive();
    }

    void GameDevGUIWindowStrategy::VBeginRender()
    {
        this->_upSt->VBeginRender();

        ::ImGuiContext* pImGuiContext = reinterpret_cast<::ImGuiContext*>(this->_pImGuiContext);
        // ImGuiのコンテキストを変更
        ::ImGui::SetCurrentContext(pImGuiContext);

#ifdef HE_USE_SDL2
        ::ImGui_ImplOpenGL3_NewFrame();
        ::ImGui_ImplSDL2_NewFrame();
#endif

        ::ImGui::NewFrame();
    }

    void GameDevGUIWindowStrategy::VEndRender()
    {
        ::ImGui::Render();
        ::ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        this->_upSt->VEndRender();
    }

}  // namespace GameDevGUI
