#include "Window.h"

// パッケージ
#include "GL/glew.h"
#include "SDL2/SDL.h"

namespace PlatformSDL2
{
    SDL2WindowStrategy::SDL2WindowStrategy(const Core::Common::Handle in_handle,
                                           const Platform::WindowConfig& in_rConfig,
                                           Context in_context)
        : Platform::WindowStrategy(in_handle, in_rConfig)
    {
        // TODO: 設定は仮
        this->_context = in_context;
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
            x = (dm.w - this->_config._uWidth) >> 1;
            y = (dm.h - this->_config._uHeight) >> 1;
        }

        // ウィンドウは最初は非表示にしておく
        auto pNewWindow =
            ::SDL_CreateWindow("", x, y, this->_config._uWidth, this->_config._uHeight,
                               SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
        SDL_GLContext pNewContext = NULL;

        // Windowに紐づいているOpenGLのコンテキストを生成
        auto [pGLContext, pWindow] = this->_context;
        if (pGLContext && pWindow)
        {
            ::SDL_DestroyWindow(reinterpret_cast<SDL_Window*>(pWindow));
            ::SDL_GL_MakeCurrent(reinterpret_cast<SDL_Window*>(pNewWindow), pGLContext);

            pNewContext = pGLContext;
        }
        // すでにコンテキストがある場合は再利用コンテキストなので設定
        else
        {
            pNewContext = ::SDL_GL_CreateContext(reinterpret_cast<SDL_Window*>(pNewWindow));
            HE_ASSERT(pNewContext);
        }

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

        this->_context = Context(NULL, NULL);
    }

    void SDL2WindowStrategy::VUpdate(const HE::Float32 in_fDt)
    {
        if (this->_onUpdteCallback) this->_onUpdteCallback(in_fDt);
    }

    void SDL2WindowStrategy::VShow()
    {
        auto [pGLContext, pWindow] = this->_context;

        ::SDL_GL_MakeCurrent(reinterpret_cast<SDL_Window*>(pWindow), pGLContext);
        ::SDL_ShowWindow(reinterpret_cast<SDL_Window*>(pWindow));
    }

    void SDL2WindowStrategy::VBeginRender()
    {
        if (this->_onBeginCallback) this->_onBeginCallback();

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

        if (this->_onEndCallback) this->_onEndCallback();

        // ウィンドウの描画バッファを切り替える
        SDL_GL_SwapWindow(reinterpret_cast<SDL_Window*>(pWindow));
    }

#ifdef HE_USE_SDL2
    void* SDL2WindowStrategy::GetWindowBySDL2() const
    {
        auto [pGLContext, pWindow] = this->_context;
        return pWindow;
    }

    void* SDL2WindowStrategy::GetContentBySDL2() const
    {
        auto [pGLContext, pWindow] = this->_context;
        return pGLContext;
    }
#endif

}  // namespace PlatformSDL2
