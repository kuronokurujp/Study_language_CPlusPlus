#include "GameDevGUIModule.h"

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

    void GameDevGUIModule::NewGUI(const Core::Common::Handle in_handle)
    {
        if (this->_pImGuiContext) this->DestoryGUI();

        IMGUI_CHECKVERSION();

        auto pImGuiContext = ::ImGui::CreateContext();
        ImGuiIO& io        = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui::StyleColorsDark();

        // SDL2用のImGUI初期化
#ifdef HE_USE_SDL2
        auto pRenderModule = this->GetDependenceModule<Render::RenderModule>();
        auto pRenderWindow = pRenderModule->GetWindow(in_handle);

        SDL_Window* pWindow = reinterpret_cast<SDL_Window*>(pRenderWindow->GetWindowBySDL2());
        void* pContext      = pRenderWindow->GetContentBySDL2();

        ::ImGui_ImplSDL2_InitForOpenGL(pWindow, pContext);
        // TODO: OpenGL名取得はwindowクラスに持った方がいいかな?
        auto pPlatformModule = HE_ENGINE.PlatformModule();
        ::ImGui_ImplOpenGL3_Init(pPlatformModule->GetOpenGLVersionNameBySDL2());

        // ウィンドウの描画開始と終了のコールバック
        pRenderWindow->SetBeginRenderCallback(
            [this, pImGuiContext]()
            {
                // ImGuiのコンテキストを変更
                ::ImGui::SetCurrentContext(pImGuiContext);

                ::ImGui_ImplOpenGL3_NewFrame();
                ::ImGui_ImplSDL2_NewFrame();

                ::ImGui::NewFrame();

                // TODO: GUI処理を記述する
                //::ImGui::Button("test");
            });
        pRenderWindow->SetEndRenderCallback(
            [this]()
            {
                ImGui::Render();
                ::ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            });
#endif

        this->_pImGuiContext = pImGuiContext;
        this->_windowHandle  = in_handle;
    }

    void GameDevGUIModule::DestoryGUI()
    {
        // 作成したフレームを破棄

        if (this->_pImGuiContext == NULL) return;

        auto pRenderModule = this->GetDependenceModule<Render::RenderModule>();
        auto pRenderWindow = pRenderModule->GetWindow(this->_windowHandle);
        pRenderWindow->SetBeginRenderCallback(NULL);
        pRenderWindow->SetEndRenderCallback(NULL);

        ::ImGuiContext* pImgGuiContext = reinterpret_cast<::ImGuiContext*>(this->_pImGuiContext);

#ifdef HE_USE_SDL2
        ::ImGui_ImplOpenGL3_Shutdown();
        ::ImGui_ImplSDL2_Shutdown();
#endif
        ::ImGui::DestroyContext(pImgGuiContext);

        this->_pImGuiContext = NULL;
        this->_windowHandle.Clear();
    }

    /// <summary>
    /// モジュール初期化
    /// </summary>
    HE::Bool GameDevGUIModule::_VStart()
    {
        this->_pImGuiContext = NULL;

        auto pPlatformModule = HE_ENGINE.PlatformModule();
        pPlatformModule->VInput()->SetInputEventCallback(
            [this](void* in_pEvent)
            {
                if (this->_pImGuiContext == NULL) return;
#ifdef HE_USE_SDL2
                SDL_Event* pEvent = reinterpret_cast<SDL_Event*>(in_pEvent);
                ImGui_ImplSDL2_ProcessEvent(pEvent);
#endif
            });

        return TRUE;
    }

    /// <summary>
    /// インスタンス破棄時に呼ばれる
    /// </summary>
    HE::Bool GameDevGUIModule::_VRelease()
    {
        this->DestoryGUI();

        return TRUE;
    }

    void GameDevGUIModule::_VUpdate(const HE::Float32 in_fDeltaTime)
    {
    }

}  // namespace GameDevGUI
