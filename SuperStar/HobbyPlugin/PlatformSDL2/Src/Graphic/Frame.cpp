#include "PlatformSDL2/Graphic/Frame.h"

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

    Frame::Frame(const Platform::FrameConfig& in_rConfig,
                 Core::Memory::UniquePtr<Platform::EventFrameInterface> in_upEvent,
                 EventInputObjectGetter in_inputInterfaceGetterFunc,
                 EventMenuCallback in_eventMenuCallback, Context in_context)
        : Platform::FrameBase(in_rConfig, std::move(in_upEvent)),
          _inputInterfaceGetterFunc(std::move(in_inputInterfaceGetterFunc)),
          _eventMenuCallback(std::move(in_eventMenuCallback)),
          _context(in_context)
    {
    }

    void Frame::VBegin()
    {
        // windowを作成
        // openglを扱うようにする
        SDL_DisplayMode dm;

        // ディスプレイ0の情報を取得
        int x         = 0;
        int y         = 0;
        auto& rConfig = this->GetConfig();
        if (::SDL_GetCurrentDisplayMode(0, &dm) == 0)
        {
            x = (dm.w - rConfig.Width()) >> 1;
            y = (dm.h - rConfig.Height()) >> 1;
        }

        // ウィンドウは最初は非表示にしておく

        Uint32 uFlags = SDL_WINDOW_OPENGL;
        if (this->_bShow == FALSE)
        {
            uFlags |= SDL_WINDOW_HIDDEN;
        }

        // 設定フラグに応じてウィンドウフラグを設定
        {
            auto eConfigFlags = rConfig.Flags();
            if (eConfigFlags & Platform::FrameConfig::EFlags_Resizable)
                uFlags |= SDL_WINDOW_RESIZABLE;
            if (eConfigFlags & Platform::FrameConfig::EFlags_Maximized)
                uFlags |= SDL_WINDOW_MAXIMIZED;
            if (eConfigFlags & Platform::FrameConfig::EFlags_Minimized)
                uFlags |= SDL_WINDOW_MINIMIZED;
        }

        auto pNewWindow = ::SDL_CreateWindow("", x, y, rConfig.Width(), rConfig.Height(), uFlags);
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
        auto mMenuItemMap = rConfig.GetMenuItemMap();
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
        if (rConfig.Flags() & Platform::FrameConfig::EFlags_WinDisableCloseBtn)
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
        auto inputHandle = rConfig.InputHandle();
        if (inputHandle.Null() == FALSE)
        {
            Platform::InputObject* pInputObj = this->_inputInterfaceGetterFunc(inputHandle);
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
                            this->_OnMenuItem(EMenuItemID::EMenuItemID_Exit);
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
                                        this->_OnMenuItem(EMenuItemID::EMenuItemID_Exit);
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

    void Frame::VEnd()
    {
        // ウィンドウのメニューを削除
#ifdef HE_WIN
        if (this->_hMenuBar)
        {
            ::DestroyMenu(static_cast<HMENU>(this->_hMenuBar));
            this->_hMenuBar = NULL;
        }
#endif

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
        // TODO: 解放処理が解放
        this->_inputInterfaceGetterFunc = NULL;
        this->_eventMenuCallback        = NULL;
    }

    void Frame::VUpdate(const HE::Float32 in_dt)
    {
    }

    void Frame::VDraw(Platform::MapDrawable& in_mDrawable)
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
        // TODO: 描画イベントを呼ぶ
        if (this->_upEvent)
        {
            this->_upEvent->VDraw(in_mDrawable);
        }

        // ウィンドウの描画バッファを切り替える
        SDL_GL_SwapWindow(reinterpret_cast<SDL_Window*>(pWindow));
    }

    void Frame::VSetPos(const HE::Uint32 in_uX, const HE::Uint32 in_uY)
    {
        auto [pGLContext, pWindow] = this->_context;
        ::SDL_SetWindowPosition(reinterpret_cast<SDL_Window*>(pWindow), in_uX, in_uY);
    }

    void Frame::VActive()
    {
        auto [pGLContext, pWindow] = this->_context;
        HE_ASSERT_RETURN(pWindow);

        ::SDL_GL_MakeCurrent(reinterpret_cast<SDL_Window*>(pWindow), pGLContext);
    }

    void Frame::VShow()
    {
        auto [pGLContext, pWindow] = this->_context;
        HE_ASSERT_RETURN(pWindow);

        this->_bShow = TRUE;
        if (this->_windowID == 0) return;

        ::SDL_ShowWindow(reinterpret_cast<SDL_Window*>(pWindow));
    }

    void Frame::VHide()
    {
        auto [pGLContext, pWindow] = this->_context;
        HE_ASSERT_RETURN(pWindow);

        this->_bShow = TRUE;
        if (this->_windowID == 0) return;

        ::SDL_HideWindow(reinterpret_cast<SDL_Window*>(pWindow));
    }

    /// <summary>
    /// メニューアイテムを追加
    /// </summary>
    const HE::Bool Frame::_AddMenuItem(const HE::Uint32 in_uId,
                                       Platform::FrameConfig::MenuItem& in_rMenuItem)
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

    void* Frame::VGetWindowBySDL2() const
    {
        auto [pGLContext, pWindow] = this->_context;
        return pWindow;
    }

    void* Frame::VGetContentBySDL2() const
    {
        auto [pGLContext, pWindow] = this->_context;
        return pGLContext;
    }
#endif

    /// <summary>
    /// メニューアイテムを押した
    /// </summary>
    void Frame::_OnMenuItem(const HE::Uint32 in_uID)
    {
        auto& rConfig = this->GetConfig();
        this->_eventMenuCallback(in_uID, rConfig);
    }
}  // namespace PlatformSDL2
