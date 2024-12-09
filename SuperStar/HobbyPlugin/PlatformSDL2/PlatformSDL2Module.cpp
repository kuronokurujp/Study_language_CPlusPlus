#include "PlatformSDL2Module.h"

#include "Engine/Memory/Memory.h"
#include "PlatformSDL2/SDL2File.h"
#include "PlatformSDL2/SDL2Input.h"
#include "PlatformSDL2/SDL2Screen.h"
#include "PlatformSDL2/SDL2System.h"
#include "PlatformSDL2/SDL2Time.h"

#include "SDL2/SDL.h"

// 依存モジュール
#include "RenderModule.h"

namespace PlatformSDL2
{
    PlatformSDL2Module::PlatformSDL2Module() : PlatformModule()
    {
        this->_AppendDependenceModule<Render::RenderModule>();
    }

    /// <summary>
    /// モジュール初期化
    /// </summary>
    HE::Bool PlatformSDL2Module::_VStart()
    {
        // SDLの初期化
        // 初期化にも色々な種類があるが、いったんVideoのみで
        Uint32 ulSDLResult = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
        HE_ASSERT_RETURN_VALUE(FALSE, ulSDLResult == 0 && "SDLの初期化に失敗");

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

            // 描画計算をGPU任せにする(0ならGPU任せになる)
            SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        }

        // プラットフォームの各機能を生成
        {
            this->_spTime   = HE_MAKE_CUSTOM_SHARED_PTR((::PlatformSDL2::Time));
            this->_spInput  = HE_MAKE_CUSTOM_SHARED_PTR((::PlatformSDL2::Input));
            this->_spFile   = HE_MAKE_CUSTOM_SHARED_PTR((::PlatformSDL2::File));
            this->_spScreen = HE_MAKE_CUSTOM_SHARED_PTR((::PlatformSDL2::Screen), this);
            this->_spSysmte = HE_MAKE_CUSTOM_SHARED_PTR((::PlatformSDL2::System));
        }

        HE::Bool bRet = TRUE;
        return TRUE;
    }

    /// <summary>
    /// インスタンス破棄時に呼ばれる
    /// </summary>
    HE::Bool PlatformSDL2Module::_VRelease()
    {
        this->_spTime.reset();
        this->_spInput.reset();
        this->_spFile.reset();
        this->_spScreen.reset();
        this->_spSysmte.reset();

        SDL_Quit();

        return TRUE;
    }

    HE::Bool PlatformSDL2Module::_VBeforeUpdate(const HE::Float32 in_fDeltaTime)
    {
        // 入力更新
        this->_spInput->VUpdate(in_fDeltaTime);

        return (this->_bQuit == FALSE);
    }

}  // namespace PlatformSDL2
