#include "Window.h"

// パッケージ
#include "GL/glew.h"
#include "SDL2/SDL.h"

namespace PlatformSDL2
{
    SDL2WindowStrategy::SDL2WindowStrategy(const Platform::WindowConfig& in_rConfig,
                                           void* in_pGLContext)
        : Platform::WindowStrategy(in_rConfig)
    {
        // TODO: 設定は仮
        this->_pContext = in_pGLContext;
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
        this->_pWindow = ::SDL_CreateWindow("", x, y, this->_config._uWidth, this->_config._uHeight,
                                            SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);

        // Windowに紐づいているOpenGLのコンテキストを生成
        if (this->_pContext == NULL)
        {
            this->_pContext = ::SDL_GL_CreateContext(reinterpret_cast<SDL_Window*>(this->_pWindow));
            HE_ASSERT(this->_pContext);
        }
        // すでにコンテキストがある場合は再利用コンテキストなので設定
        else
        {
            ::SDL_GL_MakeCurrent(reinterpret_cast<SDL_Window*>(this->_pWindow), this->_pContext);
        }

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
    }

    void SDL2WindowStrategy::VEnd()
    {
        if (this->_pContext)
        {
            ::SDL_GL_DeleteContext(this->_pContext);
        }

        if (this->_pWindow)
        {
            ::SDL_DestroyWindow(reinterpret_cast<SDL_Window*>(this->_pWindow));
            this->_pWindow = NULL;
        }
    }

    void SDL2WindowStrategy::VShow()
    {
        ::SDL_GL_MakeCurrent(reinterpret_cast<SDL_Window*>(this->_pWindow), this->_pContext);
        ::SDL_ShowWindow(reinterpret_cast<SDL_Window*>(this->_pWindow));
    }

    void SDL2WindowStrategy::VBeginRender()
    {
        // カラーバッファをクリアする
        ::glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /*

        // 3D描画開始
        ::glEnable(GL_DEPTH_TEST);
        ::glDisable(GL_BLEND);
        */
    }

    void SDL2WindowStrategy::VEndRender()
    {
        // ウィンドウの描画バッファを切り替える
        SDL_GL_SwapWindow(reinterpret_cast<SDL_Window*>(this->_pWindow));
    }
}  // namespace PlatformSDL2
