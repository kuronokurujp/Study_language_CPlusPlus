#include "PlatformSDL2/SDL2Screen.h"

#include "Engine/Common/PoolManager.h"
#include "PlatformSDL2/SDL2Font.h"
#include "PlatformSDL2/Screen/Draw/Material.h"
#include "PlatformSDL2/Screen/Draw/Mesh.h"
#include "PlatformSDL2/Screen/Draw/Texture.h"
#include "PlatformSDL2/Screen/Scene.h"
#include "PlatformSDL2/Screen/ViewPort.h"
#include "PlatformSDL2/Screen/Window.h"

// SDL2のヘッダーファイル
#include "GL/glew.h"
#include "SDL2/SDL.h"

namespace PlatformSDL2
{
    namespace Local
    {
        static SDL_Window* s_pDummyWindow    = NULL;
        static SDL_GLContext s_pShareContext = NULL;

        using PoolParticleMesh = Core::Common::RuntimePoolManager<ParticleMesh>;

    }  // namespace Local

    Screen::Screen(FontInterfaceGetterFunc in_fontInterfaceGetterFunc,
                   EventInputInterfaceGetter in_inputInterfaceGetterFunc)
    {
        this->_fontInterfaceGetterFunc  = std::move(in_fontInterfaceGetterFunc);
        this->_inputInterfaceGetterFunc = std::move(in_inputInterfaceGetterFunc);

        // ダミーのウィンドウとコンテキスト生成
        // なぜこうしているのか？
        // 利用するウィンドウを生成する前にOpenGLの命令が呼ぶケースがあるから
        // 利用するウィンドウが生成されたら消す
        Local::s_pDummyWindow =
            SDL_CreateWindow("", 0, 0, 0, 0, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
        Local::s_pShareContext =
            SDL_GL_CreateContext(reinterpret_cast<SDL_Window*>(Local::s_pDummyWindow));
        {
            // OpenGLの拡張を有効に
            glewExperimental  = GL_TRUE;
            GLenum glewResult = glewInit();
            if (glewResult != GLEW_OK)
            {
                HE_ASSERT(0);
                HE_LOG_LINE(HE_STR_TEXT("Unable to initialize GLEW: %s"),
                            ::glewGetErrorString(glewResult));
            }

            // 拡張機能の確認
            if (GLEW_ARB_texture_non_power_of_two)
            {
                HE_LOG_LINE(HE_STR_TEXT("GL_ARB_texture_non_power_of_two is supported!"));
            }
            else
            {
                HE_LOG_LINE(HE_STR_TEXT("GL_ARB_texture_non_power_of_two is NOT supported."));
            }

            GLint samples;
            glGetIntegerv(GL_SAMPLES, &samples);
            if (0 < samples)
            {
                HE_LOG_LINE(HE_STR_TEXT("Antialiasing supported with %d samples"), samples);
            }
            else
            {
                HE_LOG_LINE(HE_STR_TEXT("Antialiasing not supported."));
            }
        }
    }

    void Screen::VRelease()
    {
        for (auto it = this->_mWindowStrategy.Begin(); it != this->_mWindowStrategy.End(); ++it)
        {
            // ウィンドウの戦略を削除
            it->_data->End();
        }
        this->_mWindowStrategy.Clear();

        {
            if (Local::s_pDummyWindow)
            {
                ::SDL_DestroyWindow(reinterpret_cast<SDL_Window*>(Local::s_pDummyWindow));
                Local::s_pDummyWindow = NULL;
            }

            if (Local::s_pShareContext)
            {
                ::SDL_GL_DeleteContext(Local::s_pShareContext);
                Local::s_pShareContext = NULL;
            }
        }
    }

    void Screen::VUpdate(const HE::Float32)
    {
        // TODO: ウィンドウの開始処理をしていないならばする
        // ウィンドウはランタイム中に生成されるのでシステムが起動したとき開始処理を実行することができない
        for (auto it = this->_mWindowStrategy.Begin(); it != this->_mWindowStrategy.End(); ++it)
        {
            if (it->_data->IsBegin()) it->_data->Begin();
        }
    }

    Core::Common::Handle Screen::VCreateWindowStrategy(const Platform::WindowConfig& in_rConfig)
    {
        auto spSt = HE_MAKE_CUSTOM_UNIQUE_PTR(
            (SDL2WindowStrategy), in_rConfig,
            [this](Core::Common::Handle in_inputHandle)
            // TODO: インプットオブジェクトを返す
            {
                // TODO: 以下のアサートを使うとコンパイルエラーになる後で解決する
                // HE_ASSERT(this->_inputInterfaceGetterFunc);
                // HE_ASSERT_RETURN_VALUE(NULL, in_inputHandle.Null() == FALSE);

                return this->_inputInterfaceGetterFunc()->GetObj(in_inputHandle);
            },
            [this](const HE::Uint32 in_uId, const Platform::WindowConfig& in_rConfig)
            {
                // TODO: メインのウィンドウが消えた
                if (in_uId == SDL2WindowStrategy::EMenuItemID::EMenuItemID_Exit)
                {
                    if (in_rConfig.IsMain()) this->_bMainWindowActive = FALSE;
                }
            },
            SDL2WindowStrategy::Context(Local::s_pShareContext, Local::s_pDummyWindow));
        Local::s_pDummyWindow  = NULL;
        Local::s_pShareContext = NULL;

        if (in_rConfig.IsMain()) this->_bMainWindowActive = TRUE;

        Core::Common::Handle handle;
        handle.SetIndex(++this->_uWindowStCount, 0);

        this->_mWindowStrategy.Add(handle, std::move(spSt));

        return handle;
    }

    Platform::WindowStrategy* Screen::VGetWindow(const Core::Common::Handle in_handle)
    {
        if (this->_mWindowStrategy.Contains(in_handle))
        {
            return this->_mWindowStrategy.FindKey(in_handle)->_data.get();
        }

        return NULL;
    }

    void Screen::VRender()
    {
        ::glClearColor(0, 0, 0, 1.0f);

        // ウィンドウのレンダリングを行う
        for (auto it = this->_mWindowStrategy.Begin(); it != this->_mWindowStrategy.End(); ++it)
        {
            // TODO: ウィンドウが開始されていないのでスキップ
            if (it->_data->IsBegin()) continue;

            it->_data->Render();
        }
    }

}  // namespace PlatformSDL2
