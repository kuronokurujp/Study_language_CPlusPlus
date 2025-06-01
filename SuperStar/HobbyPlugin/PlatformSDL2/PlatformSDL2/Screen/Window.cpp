#include "Window.h"

#include "Engine/Engine.h"
#include "Engine/Platform/PlatformModule.h"

// パッケージ
#include "GL/glew.h"
#include "SDL2/SDL.h"

#ifdef HE_WIN
#include "SDL2/SDL_syswm.h"
#endif

namespace PlatformSDL2
{
    namespace Local
    {
        HWND GetSDLWinHandle(SDL_Window* win)
        {
            SDL_SysWMinfo infoWindow;
            SDL_VERSION(&infoWindow.version);
            if (::SDL_GetWindowWMInfo(win, &infoWindow) == FALSE) return NULL;

            return (infoWindow.info.win.window);
        }
    }  // namespace Local

    SDL2WindowStrategy::SDL2WindowStrategy(const Core::Common::Handle in_handle,
                                           const Platform::WindowConfig& in_rConfig,
                                           Context in_context)
        : Platform::WindowStrategy(in_handle, in_rConfig)
    {
        // TODO: 設定は仮
        this->_context = in_context;

        this->_pConfig = HE_NEW_MEM(Platform::WindowConfig, 0)(in_rConfig);
    }

    void SDL2WindowStrategy::VRelease()
    {
        HE_SAFE_DELETE_MEM(this->_pConfig);
    }

    void SDL2WindowStrategy::VBegin()
    {
        // windowを作成
        // openglを扱うようにする
        SDL_DisplayMode dm;

        // ディスプレイ0の情報を取得
        int x = 0;
        int y = 0;
        if (::SDL_GetCurrentDisplayMode(0, &dm) == 0)
        {
            x = (dm.w - this->_pConfig->Width()) >> 1;
            y = (dm.h - this->_pConfig->Height()) >> 1;
        }

        // ウィンドウは最初は非表示にしておく

        Uint32 uFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN;
        // 設定フラグに応じてウィンドウフラグを設定
        {
            auto eConfigFlags = this->_pConfig->Flags();
            if (eConfigFlags & Platform::WindowConfig::EFlags_Resizable)
                uFlags |= SDL_WINDOW_RESIZABLE;
            if (eConfigFlags & Platform::WindowConfig::EFlags_Maximized)
                uFlags |= SDL_WINDOW_MAXIMIZED;
            if (eConfigFlags & Platform::WindowConfig::EFlags_Minimized)
                uFlags |= SDL_WINDOW_MINIMIZED;
        }

        auto pNewWindow =
            ::SDL_CreateWindow("", x, y, this->_pConfig->Width(), this->_pConfig->Height(), uFlags);
        SDL_GLContext pNewContext = NULL;

        // Windowに紐づいているOpenGLのコンテキストを生成
        auto [pGLContext, pWindow] = this->_context;
        // すでにコンテキストがある場合は再利用コンテキストなので設定
        if (pGLContext && pWindow)
        {
            ::SDL_DestroyWindow(reinterpret_cast<SDL_Window*>(pWindow));
            ::SDL_GL_MakeCurrent(reinterpret_cast<SDL_Window*>(pNewWindow), pGLContext);

            pNewContext = pGLContext;
        }
        // コンテキストを新規作成
        else
        {
            pNewContext = ::SDL_GL_CreateContext(reinterpret_cast<SDL_Window*>(pNewWindow));
            HE_ASSERT(pNewContext);
        }
        this->_windowID = ::SDL_GetWindowID(pNewWindow);

        this->_context = Context(pNewContext, pNewWindow);

        // コンテキストが生成されるたびに実行しないといけない
        {
            GLenum glewResult = ::glewInit();
            if (glewResult != GLEW_OK)
            {
                HE_ASSERT(0);
                HE_LOG_LINE(HE_STR_TEXT("Unable to initialize GLEW: %s"),
                            glewGetErrorString(glewResult));
            }
        }

        // フレーム交換の垂直同期を有効にしている
        // これをしないとフレームずれが起きて表示がぶれることがあった
        if (::SDL_GL_SetSwapInterval(-1) != 0)
        {
            if (::SDL_GL_SetSwapInterval(1) != 0)
            {
                HE_LOG_LINE(HE_STR_TEXT("SDL_Error(%d)"), ::SDL_GetError());
                HE_ASSERT(FALSE);
            }
        }

        // ウィンドウのメニューを作成
#ifdef HE_WIN
        this->_hMenuBar = ::CreateMenu();
#endif
        // メニュー項目を追加
        auto mMenuItemMap = this->_pConfig->GetMenuItemMap();
        {
            for (auto itr = mMenuItemMap.Begin(); itr != mMenuItemMap.End(); ++itr)
            {
                this->_AddMenuItem(itr->_key, itr->_data);
            }
        }

#ifdef HE_WIN
        if (0 < mMenuItemMap.Size())
        {
            auto hWnd = Local::GetSDLWinHandle(pNewWindow);
            ::SetMenu(hWnd, static_cast<HMENU>(this->_hMenuBar));
        }

        // ウィンドウの右側の×ボタンを消すかどうか
        if (this->_pConfig->Flags() & Platform::WindowConfig::EFlags_WinDisableCloseBtn)
        {
            auto hWnd   = Local::GetSDLWinHandle(pNewWindow);
            HMENU hMenu = ::GetSystemMenu(hWnd, FALSE);
            if (hMenu != NULL)
            {
                ::DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
            }
        }
#endif
        // 入力イベントを取得してウィンドウの入力処理
        auto inputHandle = this->_pConfig->InputHandle();
        if (inputHandle.Null() == FALSE)
        {
            auto* pInputObj = HE_ENGINE.PlatformModule()->VInput()->GetObj(inputHandle);
            pInputObj->SetEventCallback(
                [this](void* in_pEvent)
                {
                    const SDL_Event* pSDLEvent = reinterpret_cast<const SDL_Event*>(in_pEvent);
                    HE_ASSERT(pSDLEvent);
                    switch (pSDLEvent->type)
                    {
                        // アプリ全体の終了イベント
                        // ウィンドウが一つのみならウィンドウの×ボタンをクリックすると呼ばれる
                        // でもウィンドウが複数あるとよばれない
                        case SDL_QUIT:
                        {
                            this->_bClose = TRUE;
                            break;
                        }
                        case SDL_WINDOWEVENT:
                        {
                            // ウィンドウが複数時の処理
                            switch (pSDLEvent->window.event)
                            {
                                case SDL_WINDOWEVENT_CLOSE:
                                {
                                    if (pSDLEvent->window.windowID == this->_windowID)
                                        this->_bClose = TRUE;
                                    break;
                                }
                            }
                            break;
                        }
#ifdef HE_WIN
                            // ウィンドウのメニューバーの入力取得
                        case SDL_SYSWMEVENT:
                        {
                            if (pSDLEvent->syswm.msg->msg.win.msg == WM_COMMAND)
                            {
                                // 登録したメニュー項目が押されたらコールバックを呼び出す
                                auto uID = static_cast<HE::Uint32>(
                                    LOWORD(pSDLEvent->syswm.msg->msg.win.wParam));
                                this->_OnMenuItem(uID);
                            }

                            if (pSDLEvent->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                            {
                                int newWidth  = pSDLEvent->window.data1;
                                int newHeight = pSDLEvent->window.data2;
                                ::glViewport(0, 0, newWidth, newHeight);
                            }

                            break;
                        }
#endif
                    }
                });
        }
    }

    void SDL2WindowStrategy::VEnd()
    {
        auto [pGLContext, pWindow] = this->_context;
        if (pGLContext)
        {
            ::SDL_GL_DeleteContext(pGLContext);
        }

        if (pWindow)
        {
            ::SDL_DestroyWindow(reinterpret_cast<SDL_Window*>(pWindow));
        }

        this->_context  = Context(NULL, NULL);
        this->_windowID = 0;
    }

    void SDL2WindowStrategy::VUpdate(const HE::Float32 in_dt)
    {
        // メインウィンドウ終了はエンジンの終了
        if (this->_bClose)
        {
            if (this->_pConfig->IsMain()) HE_ENGINE.Quit();
        }
    }

    void SDL2WindowStrategy::VSetPos(const HE::Uint32 in_uX, const HE::Uint32 in_uY)
    {
        auto [pGLContext, pWindow] = this->_context;
        ::SDL_SetWindowPosition(reinterpret_cast<SDL_Window*>(pWindow), in_uX, in_uY);
    }

    void SDL2WindowStrategy::VActive()
    {
        auto [pGLContext, pWindow] = this->_context;
        ::SDL_GL_MakeCurrent(reinterpret_cast<SDL_Window*>(pWindow), pGLContext);
    }

    void SDL2WindowStrategy::VShow()
    {
        auto [pGLContext, pWindow] = this->_context;

        ::SDL_ShowWindow(reinterpret_cast<SDL_Window*>(pWindow));
    }

    void SDL2WindowStrategy::VHide()
    {
        auto [pGLContext, pWindow] = this->_context;

        ::SDL_HideWindow(reinterpret_cast<SDL_Window*>(pWindow));
    }

    void SDL2WindowStrategy::VBeginRender()
    {
        auto [pGLContext, pWindow] = this->_context;

        // カラーバッファをクリアする
        ::glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 3D描画開始
        //::glEnable(GL_DEPTH_TEST);
        //::glDepthFunc(GL_LEQUAL);
        //::glDepthRange(0.0, 1.0);  // 深度範囲の設定
        //::glDisable(GL_BLEND);
    }

    void SDL2WindowStrategy::VEndRender()
    {
        auto [pGLContext, pWindow] = this->_context;

        // ウィンドウの描画バッファを切り替える
        SDL_GL_SwapWindow(reinterpret_cast<SDL_Window*>(pWindow));
    }

    /// <summary>
    /// メニューアイテムを追加
    /// </summary>
    const HE::Bool SDL2WindowStrategy::_AddMenuItem(
        const HE::Uint32 in_uId, Platform::WindowConfig::WindowMenuItem& in_rMenuItem)
    {
        // windowsでは文字列型がWChar型でないといけない
#ifdef HE_WIN
        if (this->_hMenuBar != NULL)
        {
            Core::Common::g_szTempFixedString128 = in_rMenuItem.szName;
            HE::WChar szName[128]                = HE_STR_W_TEXT("");
            Core::Common::g_szTempFixedString128.OutputW(szName, HE_ARRAY_NUM(szName));

            auto hMenuBar = static_cast<HMENU>(this->_hMenuBar);
            {
                ::AppendMenu(hMenuBar, MF_POPUP, in_uId, szName);
            }
        }
#endif

        return TRUE;
    }

#ifdef HE_USE_SDL2

    void* SDL2WindowStrategy::VGetWindowBySDL2() const
    {
        auto [pGLContext, pWindow] = this->_context;
        return pWindow;
    }

    void* SDL2WindowStrategy::VGetContentBySDL2() const
    {
        auto [pGLContext, pWindow] = this->_context;
        return pGLContext;
    }
#endif

    /// <summary>
    /// メニューアイテムを押した
    /// </summary>
    void SDL2WindowStrategy::_OnMenuItem(const HE::Uint32 in_uID)
    {
        auto mMenuItemMap = this->_pConfig->GetMenuItemMap();
        if (mMenuItemMap.Contains(in_uID))
        {
            auto itr = mMenuItemMap.FindKey(in_uID);
            this->_eventMenuCallback(in_uID);
        }
    }
}  // namespace PlatformSDL2
