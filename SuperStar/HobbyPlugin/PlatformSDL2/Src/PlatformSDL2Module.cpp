#include "PlatformSDL2Module.h"

// SDL2プラットフォームの機能
#include "PlatformSDL2/SDL2File.h"
#include "PlatformSDL2/SDL2Font.h"
#include "PlatformSDL2/SDL2Input.h"
#include "PlatformSDL2/SDL2Graphic.h"
#include "PlatformSDL2/SDL2System.h"
#include "PlatformSDL2/SDL2Time.h"

// パッケージ
#include "GL/glew.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

namespace PlatformSDL2
{
#ifdef HE_ENGINE_DEBUG
    /// <summary>
    /// OpenGLのバグが起きた時のメッセージ
    /// </summary>
    void GLAPIENTRY _ShowMessageFromGLError(GLenum in_source, GLenum in_type, GLuint in_id,
                                            GLenum in_severity, GLsizei in_length,
                                            const GLchar* in_pMessage, const void* in_pUserParam)
    {
        HE_LOG_LINE(HE_STR_TEXT("OpenGL Debug Message: %s"), in_pMessage);
    }
#endif

    PlatformSDL2Module::PlatformSDL2Module() : PlatformModule()
    {
    }

    HE::Bool PlatformSDL2Module::VIsQuit()
    {
        if (this->_bMainWindowInitialized == FALSE)
        {
            // メインウィンドウの処理が終わっている場合は終了状態
            return FALSE;
        }

        auto pGraphic = reinterpret_cast<PlatformSDL2::Graphic*>(this->_spGraphic.get());
        if (pGraphic->IsMainWindowActive())
        {
            // メインスクリーンは終了状態ではない
            // ここでTRUEを返すとエンジンが終了してしまう
            return FALSE;
        }

        return TRUE;
    }

    /// <summary>
    /// モジュール初期化
    /// </summary>
    HE::Bool PlatformSDL2Module::_VStart()
    {
        // SDLの初期化
        // 初期化にも色々な種類があるが、いったんVideoのみで
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
        {
            HE_LOG_LINE(HE_STR_TEXT("Error: SDL_Init Message= %s"), SDL_GetError());
            HE_ASSERT_RETURN_VALUE(FALSE, FALSE && "SDLの初期化に失敗");
        }

        // SDLのフォント初期化
        if (TTF_Init() == -1)
        {
            HE_LOG_LINE(HE_STR_TEXT("Error: TTF_Init Message= %s"), TTF_GetError());

            SDL_Quit();
            HE_ASSERT_RETURN_VALUE(FALSE, FALSE && "SDL2_TTFの初期化失敗");
        }

        // openglの属性設定をする
        // windowを作成する前にしないといけない
        {
            // コア機能を有効にする
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

            // コンテキストのバージョン設定
            // OpenGL3.3を使用
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

            // RGBAそれぞれに割り当てるビットサイズ指定
            SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
            SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
            // デプスバッファのビット数を設定
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

            // 描画のダブルバッファリングを有効にする
            // これで描画くずれが防げる
            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
            // 描画計算をGPU任せにする(0ならCPU任せになる)
            SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        }

        // プラットフォームの各機能を生成
        {
            this->_spTime   = HE_MAKE_CUSTOM_SHARED_PTR((::PlatformSDL2::Time));
            this->_spInput  = HE_MAKE_CUSTOM_SHARED_PTR((::PlatformSDL2::Input));
            this->_spFile   = HE_MAKE_CUSTOM_SHARED_PTR((::PlatformSDL2::File));
            this->_spGraphic = HE_MAKE_CUSTOM_SHARED_PTR((::PlatformSDL2::Graphic),
                                                        [this]() { return this->_spFont.get(); },
                                                        [this]() { return this->_spInput.get(); });
            this->_spSysmte = HE_MAKE_CUSTOM_SHARED_PTR((::PlatformSDL2::System));
            this->_spFont =
                HE_MAKE_CUSTOM_SHARED_PTR((::PlatformSDL2::Font),
                                          [this](const Core::File::Path& in_rFilePath)
                                          { return this->_spFile->VLoadBinary(in_rFilePath); });
        }

        // ウィンドウズのWinAPIを使えるようにする
#ifdef HE_WIN
        SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
#endif

        {
#ifdef HE_ENGINE_DEBUG
            const GLubyte* pVersion = ::glGetString(GL_VERSION);
            HE_LOG_LINE(HE_STR_TEXT("OpenGL Version(%d)"), *pVersion);

            GLint maxTextureSize;
            ::glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
            HE_LOG_LINE(HE_STR_TEXT("Max texture size: %d"), maxTextureSize);

            // テクスチャユニット数
            GLint textureUnits = 0;
            ::glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &textureUnits);
            HE_LOG_LINE(HE_STR_TEXT("GPU TextureUnitNum %d"), textureUnits);

#endif
        }

        {
#ifdef HE_ENGINE_DEBUG
            ::glEnable(GL_DEBUG_OUTPUT);
            ::glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(_ShowMessageFromGLError, NULL);
#endif
        }

        return TRUE;
    }

    /// <summary>
    /// インスタンス破棄時に呼ばれる
    /// </summary>
    HE::Bool PlatformSDL2Module::_VRelease()
    {
        PlatformModule::_VRelease();

        ::TTF_Quit();
        ::SDL_Quit();

        return TRUE;
    }

    void PlatformSDL2Module::_VBeforeUpdate(const HE::Float32 in_fDeltaTime)
    {
        this->_spInput->VUpdate(in_fDeltaTime);
    }

    void PlatformSDL2Module::_VLateUpdate(const HE::Float32 in_fDeltaTime)
    {
        PlatformModule::_VLateUpdate(in_fDeltaTime);

        if (this->_bMainWindowInitialized)
        {
            auto pGraphic = reinterpret_cast<PlatformSDL2::Graphic*>(this->_spGraphic.get());
            if (pGraphic->IsMainWindowActive())
            {
                this->_bMainWindowInitialized = TRUE;
            }
        }
    }

}  // namespace PlatformSDL2
