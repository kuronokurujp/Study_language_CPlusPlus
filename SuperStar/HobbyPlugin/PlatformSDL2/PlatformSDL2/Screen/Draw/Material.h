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

        // TODO: シェーダーロード/アンロード
        HE::Bool LoadShader(const HE::Char* in_pVertexMem, const HE::Char* in_pPixelMem);
        void UnLoadShader();

        /// <summary>
        /// TODO: マテリアル有効/無効
        /// </summary>
        void Enable();
        void Disable();

        // TODO: マテリアルに渡すプロパティ
        // シェーダーに行列を渡す
        void SetPropertyMatrix(const HE::UTF8* in_pName, const Core::Math::Matrix4&);
        // シェーダーにベクトルを渡す
        void SetPropertyVector3(const HE::UTF8* in_pName, const Core::Math::Vector3&);
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
            HE::Uint32 uUnicode = 0;
            //  テクスチャのUV値
            HE::Float32 _fTexSU, _fTexSV;
            HE::Float32 _fTexEU, _fTexEV;
        };

    public:
        /// <summary>
        /// フォントのマテリアルを作る
        /// </summary>
        HE::Bool Create(void* in_pSDLTexSurf, Glyph* in_aGlyps, const HE::Uint32 in_uGlyphCount);

        /// <summary>
        /// 解放
        /// </summary>
        HE::Bool VRelease() override final;

        const Glyph* GetGlyph(const HE::Uint32);

    private:
        // TODO: グリフの数が不定なので固定にはできない
        // TODO: 動的用のを作ったら差し替える
        std::unordered_map<HE::Uint32, Glyph> _mGlyphs;
    };

}  // namespace PlatformSDL2
