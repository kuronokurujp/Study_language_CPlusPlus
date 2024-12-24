#pragma once

#include <unordered_map>

#include "Engine/Math/Math.h"
#include "Engine/MiniEngine.h"

namespace PlatformSDL2
{
    class TextureBase;

    /// <summary>
    /// SDL2のマテリアル
    /// </summary>
    class Material
    {
    public:
        virtual ~Material() { this->VRelease(); }

        /// <summary>
        /// 解放
        /// </summary>
        virtual HE::Bool VRelease();

        // シェーダーロード/アンロード
        HE::Bool LoadShader(const HE::Char* in_pVertexMem, const HE::Char* in_pPixelMem);
        void UnLoadShader();

        /// <summary>
        /// マテリアル有効/無効
        /// </summary>
        void Enable();
        void Disable();

        // マテリアルに渡すプロパティ
        // シェーダーに行列を渡す
        void SetPropertyMatrix(const HE::UTF8* in_pName, const Core::Math::Matrix4&);
        // シェーダーにベクトルを渡す
        void SetPropertyVector3(const HE::UTF8* in_pName, const Core::Math::Vector3&);
        void SetPropertyVector4(const HE::UTF8* in_pName, const Core::Math::Vector4&);
        // シェーダーにFloat32に渡す
        void SetPropertyFloat(const HE::UTF8* in_pName, const HE::Float32);
        // シェーダーにテクスチャを渡す
        void SetPropertyTexture(const HE::UTF8* in_pName, const TextureBase*);

    protected:
        void* _pShader = NULL;
    };

    /// <summary>
    /// フォントマテリアル
    /// </summary>
    class FontMaterial final : public Material
    {
    public:
        struct Glyph
        {
            //  テクスチャのUV値
            HE::Float32 _fTexSU, _fTexEU;
            HE::Float32 _fTexSV, _fTexEV;
            HE::Uint32 _uW;
            HE::Uint32 _uH;
        };

        // TODO: 動的用のを作ったら差し替える
        using GlyphMap = std::unordered_map<HE::Uint32, Glyph>;

    public:
        void CopyGlyphs(GlyphMap&& in_rrGlyphs);

        /// <summary>
        /// 解放
        /// </summary>
        HE::Bool VRelease() override final;

        const Glyph* GetGlyph(const HE::Uint32);

    private:
        GlyphMap _mGlyphs;
    };

}  // namespace PlatformSDL2
