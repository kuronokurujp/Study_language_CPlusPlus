#pragma once

#include "Engine/MiniEngine.h"

namespace PlatformSDL2
{
    /// <summary>
    /// テクスチャの基本クラス
    /// </summary>
    class TextureBase
    {
    public:
        TextureBase(const HE::Uint32 in_uType);

        HE::Bool Release();

        void Enable();
        void Disable();

        HE::Uint32 Width() { return this->_uWidth; }
        HE::Uint32 Height() { return this->_uHeight; }

        const HE::Uint32 Handle() const { return this->_uTextureID; }

    private:
        void _Clear()
        {
            this->_uWidth = this->_uHeight = 0;
            this->_uTextureID              = 0;
            this->_uTextureType            = 0;
        }

    protected:
        HE::Uint32 _uTextureType;
        HE::Uint32 _uTextureID;

        HE::Uint32 _uWidth;
        HE::Uint32 _uHeight;
    };

    /// <summary>
    /// 画像テクスチャ
    /// </summary>
    class TextureImage final : public TextureBase
    {
    public:
        TextureImage(const HE::Uint32 in_uType, void* in_pImg, const HE::Uint32 in_uSize);
    };

    /// <summary>
    /// サーフェイステクスチャ
    /// </summary>
    class TextureSurface final : public TextureBase
    {
    public:
        TextureSurface(const HE::Uint32 in_uType, void* in_pPixel, const HE::Uint32 in_uWidth,
                       const HE::Uint32 in_uHeight);
    };
}  // namespace PlatformSDL2
