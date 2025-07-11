#include "PlatformSDL2/SDL2Graphic.h"

#include "Engine/Common/PoolManager.h"

// 各描画
#include "PlatformSDL2/Graphic/Draw/Material.h"
#include "PlatformSDL2/Graphic/Draw/Mesh.h"
#include "PlatformSDL2/Graphic/Draw/Texture.h"
#include "PlatformSDL2/Graphic/Frame.h"
// #include "PlatformSDL2/Graphic/Scene.h"
// #include "PlatformSDL2/Graphic/ViewPort.h"
#include "PlatformSDL2/SDL2Font.h"

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

    Graphic::Graphic(FontInterfaceGetterFunc in_fontInterfaceGetterFunc,
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

    void Graphic::VRelease()
    {
        // フレーム削除
        {
            for (auto it = this->_mFrame.Begin(); it != this->_mFrame.End(); ++it)
            {
                it->_data->VEnd();
                HE_SAFE_DELETE_UNIQUE_PTR(it->_data);
            }
            this->_mFrame.Clear();

            for (auto it = this->_mPendingFrame.Begin(); it != this->_mPendingFrame.End(); ++it)
            {
                it->_data->VEnd();
                HE_SAFE_DELETE_UNIQUE_PTR(it->_data);
            }
            this->_mPendingFrame.Clear();
        }

        // 描画を削除
        {
            for (auto it = this->_mDrawable.Begin(); it != this->_mDrawable.End(); ++it)
            {
                it->_data->VEnd();
                HE_SAFE_DELETE_UNIQUE_PTR(it->_data);
            }
            this->_mDrawable.Clear();

            for (auto it = this->_mPedingDrawable.Begin(); it != this->_mPedingDrawable.End(); ++it)
            {
                it->_data->VEnd();
                HE_SAFE_DELETE_UNIQUE_PTR(it->_data);
            }
            this->_mPedingDrawable.Clear();
        }

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

    void Graphic::VUpdate(const HE::Float32)
    {
        // TODO: ウィンドウの開始処理をしていないならばする
        // ウィンドウはランタイム中に生成されるのでシステムが起動したとき開始処理を実行することができない
        for (auto it = this->_mPedingDrawable.Begin(); it != this->_mPedingDrawable.End(); ++it)
        {
            it->_data->VBegin();

            this->_mDrawable.Add(it->_key, std::move(it->_data));
        }
        this->_mPedingDrawable.Clear();

        // TODO: ウィンドウの開始処理をしていないならばする
        // ウィンドウはランタイム中に生成されるのでシステムが起動したとき開始処理を実行することができない
        for (auto it = this->_mPendingFrame.Begin(); it != this->_mPendingFrame.End(); ++it)
        {
            it->_data->VBegin();

            this->_mFrame.Add(it->_key, std::move(it->_data));
        }
        this->_mPendingFrame.Clear();
    }

    Core::Common::Handle Graphic::VCreateFrame(
        const Platform::FrameConfig& in_rConfig,
        Core::Memory::UniquePtr<Platform::EventFrameInterface> in_upEvent)
    {
        auto upFrame = HE_MAKE_CUSTOM_UNIQUE_PTR(
            (Frame), in_rConfig, std::move(in_upEvent),
            [this](Core::Common::Handle in_inputHandle)
            // TODO: インプットオブジェクトを返す
            {
                // TODO:
                // 以下のアサートを使うとコンパイルエラーになる後で解決する
                // HE_ASSERT(this->_inputInterfaceGetterFunc);
                // HE_ASSERT_RETURN_VALUE(NULL, in_inputHandle.Null() ==
                // FALSE);

                return this->_inputInterfaceGetterFunc()->GetObj(in_inputHandle);
            },
            [this](const HE::Uint32 in_uId, const Platform::FrameConfig& in_rConfig)
            {
                // TODO: メインのウィンドウが消えた
                if (in_uId == Frame::EMenuItemID::EMenuItemID_Exit)
                {
                    if (in_rConfig.IsMain()) this->_bMainFrameActive = FALSE;
                }
            },
            Frame::Context(Local::s_pShareContext, Local::s_pDummyWindow));
        Local::s_pDummyWindow  = NULL;
        Local::s_pShareContext = NULL;

        if (in_rConfig.IsMain()) this->_bMainFrameActive = TRUE;

        Core::Common::Handle handle;
        handle.SetIndex(++this->_uFrameCount, 0);

        this->_mPendingFrame.Add(handle, std::move(upFrame));

        return handle;
    }

    Platform::FrameBase* Graphic::VGetFrame(const Core::Common::Handle in_handle)
    {
        if (this->_mFrame.Contains(in_handle))
        {
            return this->_mFrame.FindKey(in_handle)->_data.get();
        }
        else if (this->_mPendingFrame.Contains(in_handle))
        {
            return this->_mPendingFrame.FindKey(in_handle)->_data.get();
        }

        return NULL;
    }

    void Graphic::VAddDrawable(Core::Memory::UniquePtr<Platform::Drawable2DInterface> in_upEvent)
    {
        this->_mPedingDrawable.Add(&in_upEvent->VGetRTTI(), std::move(in_upEvent));
    }

    void Graphic::VDraw()
    {
        ::glClearColor(0, 0, 0, 1.0f);

        // ウィンドウのレンダリングを行う
        for (auto it = this->_mFrame.Begin(); it != this->_mFrame.End(); ++it)
        {
            it->_data->VDraw(this->_mDrawable);
        }
    }
}  // namespace PlatformSDL2
