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
        TextureBase(const HE::Uint32 in_uType, const HE::Uint32 in_uUnitID);

        HE::Bool Release();

        void Enable();
        void Disable();

        HE::Uint32 Width() { return this->_uWidth; }
        HE::Uint32 Height() { return this->_uHeight; }

        const HE::Uint32 Handle() const { return this->_uUnitIdx; }

    private:
        void _Clear()
        {
            this->_uWidth = this->_uHeight = 0;
            this->_uTextureID              = 0;
            this->_uTextureType            = 0;
            this->_uUnitID                 = 0;
            this->_uUnitIdx                = 0;
        }

    protected:
        HE::Uint32 _uTextureType = 0;
        unsigned int _uTextureID = 0;
        HE::Uint32 _uUnitID      = 0;
        HE::Uint32 _uUnitIdx     = 0;

        HE::Uint32 _uWidth  = 0;
        HE::Uint32 _uHeight = 0;
    };

    /// <summary>
    /// 画像テクスチャ
    /// </summary>
    class TextureImage final : public TextureBase
    {
    public:
        TextureImage(const HE::Uint32 in_uType, const HE::Uint32 in_uUnitID, void* in_pImg,
                     const HE::Uint32 in_uSize);
    };

    /// <summary>
    /// サーフェイステクスチャ
    /// </summary>
    class TextureSurface final : public TextureBase
    {
    public:
        TextureSurface(const HE::Uint32 in_uType, const HE::Uint32 in_uUnitID, void* in_pPixel,
                       const HE::Uint32 in_uWidth, const HE::Uint32 in_uHeight);
    };
}  // namespace PlatformSDL2
