#include "PlatformSDL2/Graphic/Draw/Texture.h"

#include "GL/glew.h"
#include "SDL2/SDL.h"
// 画像を扱うため必要
#include "SOIL2/SOIL2.h"

namespace PlatformSDL2
{

    TextureBase::TextureBase(const HE::Uint32 in_uType, const HE::Uint32 in_uUnitID)
    {
        this->_Clear();

        this->_uTextureType = in_uType;
        this->_uUnitID      = in_uUnitID;
        this->_uUnitIdx     = this->_uUnitID - GL_TEXTURE0;
    }

    HE::Bool TextureBase::Release()
    {
        // 二重開放防止
        if (this->_uTextureID == 0)
        {
            return TRUE;
        }

        glDeleteTextures(1, reinterpret_cast<GLuint*>(&this->_uTextureID));
        this->_uTextureID = 0;
        this->_uUnitID    = 0;

        return TRUE;
    }

    void TextureBase::Enable()
    {
        // 画像を書き込むためにテクスチャを有効化
        glActiveTexture(this->_uUnitID);
        HE_ASSERT(glGetError() == GL_NO_ERROR);

        ::glBindTexture(this->_uTextureType, this->_uTextureID);

        // 使うテクスチャーの縦横
        GLint width, height;
        glGetTexLevelParameteriv(this->_uTextureType, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(this->_uTextureType, 0, GL_TEXTURE_HEIGHT, &height);
    }

    void TextureBase::Disable()
    {
        ::glBindTexture(this->_uTextureType, 0);
    }

    TextureImage::TextureImage(const HE::Uint32 in_uType, const HE::Uint32 in_uUnitID,
                               void* in_pImg, const HE::Uint32 in_uSize)
        : TextureBase(in_uType, in_uUnitID)
    {
        HE_ASSERT_RETURN(in_pImg);
        HE_ASSERT_RETURN(0 < in_uSize);

        // テクスチャユニット数
        {
            GLint textureUnits = 0;
            ::glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &textureUnits);
            HE_LOG_LINE(HE_STR_TEXT("GPU TextureUnitNum %d"), textureUnits);
        }

        // OpneGLのテクスチャを作成
        ::glGenTextures(1, reinterpret_cast<GLuint*>(&this->_uTextureID));

        ::glBindTexture(this->_uTextureType, this->_uTextureID);
        auto error = ::glGetError();
        if (error != GL_NO_ERROR)
        {
            HE_LOG_LINE(HE_STR_TEXT("OpenGL Error: 0x%X"), error);
            HE_ASSERT(FALSE);
            return;
        }

        // メモリ上の画像データをSOILでロード
        int width, height, channels;
        unsigned char* pImage =
            ::SOIL_load_image_from_memory(reinterpret_cast<const unsigned char*>(in_pImg), in_uSize,
                                          &width, &height, &channels, SOIL_LOAD_RGBA);

        if (pImage == NULL)
        {
            HE_LOG_LINE(HE_STR_TEXT("SOIL_load_image_from_memory Error: %s"), ::SOIL_last_result());
            HE_ASSERT(FALSE);
            this->Release();

            return;
        }

        int format = GL_RGB;
        if (channels == 4)
        {
            format = GL_RGBA;
        }

        // OpenGLで作成したテクスチャにファイルロードした画像を当てはめる
        ::glTexImage2D(
            // ターゲット
            this->_uTextureType,
            // LoD
            0,
            // カラーフォーマット
            format, width, height, 0,
            // 入力データのカラーフォーマット
            format,
            // 入力データのビット深度
            GL_UNSIGNED_BYTE,
            // 画像データのポインタ
            in_pImg);

        // テクスチャデータを作成したのでファイルロードした画像はいらないので破棄
        SOIL_free_image_data(pImage);

        error = ::glGetError();
        if (error != GL_NO_ERROR)
        {
            HE_LOG_LINE(HE_STR_TEXT("OpenGL Error: 0x%X"), error);
            HE_ASSERT(FALSE);
            this->Release();

            return;
        }

        // テクスチャのフィルタリングをバイリニアにする
        ::glTexParameteri(this->_uTextureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        ::glTexParameteri(this->_uTextureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        ::glTexParameteri(this->_uTextureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        ::glTexParameteri(this->_uTextureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        if (::glGetError() != GL_NO_ERROR)
        {
            HE_ASSERT(FALSE);
            this->Release();

            return;
        }

        HE_LOG_LINE(HE_STR_TEXT("enable textureID: flag:%d / id:%d"),
                    ::glIsTexture(this->_uTextureID), this->_uTextureID);

        this->Disable();

        this->_uWidth  = width;
        this->_uHeight = height;

#ifdef HE_ENGINE_DEBUG
        // 画像データを表示
        HE_LOG_LINE(HE_STR_TEXT("Tex Width(%d), Height(%d)"), this->_uWidth, this->_uHeight);
        HE_LOG_LINE(HE_STR_TEXT("Tex channels(%d)"), channels);

#endif
    }

    TextureSurface::TextureSurface(const HE::Uint32 in_uType, const HE::Uint32 in_uUnitID,
                                   void* in_pPixel, const HE::Uint32 in_uWidth,
                                   const HE::Uint32 in_uHeight)
        : TextureBase(in_uType, in_uUnitID)
    {
        HE_ASSERT(in_pPixel);

        // テクスチャ生成
        ::glGenTextures(1, &this->_uTextureID);
        ::glBindTexture(this->_uTextureType, this->_uTextureID);

        // 生成したテクスチャをピクセルに書き込む
        ::glTexImage2D(this->_uTextureType, 0, GL_RGBA, in_uWidth, in_uHeight, 0, GL_RGBA,
                       GL_UNSIGNED_BYTE, in_pPixel);
        auto error = ::glGetError();
        if (error != GL_NO_ERROR)
        {
            HE_LOG_LINE(HE_STR_TEXT("OpenGL Error: 0x%X"), error);
            HE_ASSERT(FALSE);
            return;
        }
        // テクスチャのフィルタリングをバイリニアにする
        ::glTexParameteri(this->_uTextureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        ::glTexParameteri(this->_uTextureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // ラップモードを設定
        ::glTexParameteri(this->_uTextureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        ::glTexParameteri(this->_uTextureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        if (::glGetError() != GL_NO_ERROR)
        {
            HE_ASSERT(FALSE);
            this->Release();

            return;
        }

        this->_uWidth  = in_uWidth;
        this->_uHeight = in_uHeight;
    }

}  // namespace PlatformSDL2
