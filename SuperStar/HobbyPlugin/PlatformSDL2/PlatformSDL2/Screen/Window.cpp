#include "Window.h"

// パッケージ
#include "GL/glew.h"
#include "SDL2/SDL.h"

namespace PlatformSDL2
{
    /// <summary>
    /// DXLibのウィンドウロジック
    /// </summary>
    DXLibWindowStrategy::DXLibWindowStrategy()
    {
        // TODO: 設定は仮
        this->_uViewPortCount = 1;
        this->_uHeight        = 480;
        this->_uWidht         = 640;
    }

    void DXLibWindowStrategy::_VBegin()
    {
        // windowを作成
        // openglを扱うようにする
        SDL_DisplayMode dm;

        // ディスプレイ0の情報を取得
        int x = 0;
        int y = 0;
        if (SDL_GetCurrentDisplayMode(0, &dm) == 0)
        {
            x = (dm.w - this->_uWidht) >> 1;
            y = (dm.h - this->_uHeight) >> 1;
        }

        // ウィンドウは最初は非表示にしておく
        this->_pWindow = SDL_CreateWindow("", x, y, this->_uWidht, this->_uHeight,
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);

        // Windowに紐づいているOpenGLのコンテキストを生成
        this->_pContext = SDL_GL_CreateContext(reinterpret_cast<SDL_Window*>(this->_pWindow));
        HE_ASSERT(this->_pContext);

        // コンテキストが生成されるたびに実行しないといけない
        {
            GLenum glewResult = glewInit();
            if (glewResult != GLEW_OK)
            {
                HE_ASSERT(0);
                HE_LOG_LINE(HE_STR_TEXT("Unable to initialize GLEW: %s"),
                            glewGetErrorString(glewResult));
            }
        }
    }

    void DXLibWindowStrategy::_VEnd()
    {
        if (this->_pContext)
        {
            SDL_GL_DeleteContext(this->_pContext);
        }

        if (this->_pWindow)
        {
            SDL_DestroyWindow(reinterpret_cast<SDL_Window*>(this->_pWindow));
            this->_pWindow = NULL;
        }
    }

    void DXLibWindowStrategy::_VShow()
    {
        SDL_ShowWindow(reinterpret_cast<SDL_Window*>(this->_pWindow));
    }

    void DXLibWindowStrategy::_VBeginRender()
    {
        // カラーバッファをクリアする
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        /*

        // 3D描画開始
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        */
    }

    void DXLibWindowStrategy::_VEndRender()
    {
        // ウィンドウの描画バッファを切り替える
        SDL_GL_SwapWindow(reinterpret_cast<SDL_Window*>(this->_pWindow));
    }

}  // namespace PlatformSDL2
