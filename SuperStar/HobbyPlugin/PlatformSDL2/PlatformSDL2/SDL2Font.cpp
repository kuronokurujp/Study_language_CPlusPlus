#include "SDL2Font.h"

#include <GL/glew.h>

#include "./Screen/Draw/Material.h"
#include "./Screen/Draw/Texture.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "freetype/freetype.h"

// 依存モジュール
#include "PlatformSDL2Module.h"

namespace PlatformSDL2
{
    struct FontBuildData
    {
        HE::Char* szAllChar          = NULL;
        FontMaterial::Glyph* pGlyphs = NULL;
        HE::Uint32 uCharCount        = 0;
        HE::Uint32 uGlyphCount       = 0;
    };

    /// <summary>
    /// TODO: フォントの構築に必要なデータを生成
    /// </summary>
    static FontBuildData _CreateFontBuildData(void* in_pFontData, const HE::Uint32 in_uSize)
    {
        FT_Face face;
        FT_Library library;

        FontBuildData buildData;

        // FreeType ライブラリを初期化
        if (::FT_Init_FreeType(&library))
        {
            HE_LOG_LINE(HE_STR_TEXT("FreeTypeのライブラリ初期化に失敗"));
            return buildData;
        }

        // メモリ上のフォントデータを利用して Face を作成
        if (::FT_New_Memory_Face(library, reinterpret_cast<FT_Byte*>(in_pFontData), in_uSize, 0,
                                 &face))
        {
            HE_LOG_LINE(HE_STR_TEXT("FreeTypeのFace生成が失敗"));
            ::FT_Done_FreeType(library);

            return buildData;
        }

        // フォント内の文字を列挙
        ::FT_UInt freeTypeGlyphIndex = 0;
        ::FT_ULong unicode           = 0;
        HE::Uint32 uCharCount        = 0;
        HE::Uint32 uGlyphCount       = 0;
        // まずどれくらい文字バッファサイズが必要かバッファ数を計算
        {
            ::FT_Get_First_Char(face, &freeTypeGlyphIndex);

            while (freeTypeGlyphIndex != 0)
            {
                if (unicode <= 0x7F)
                {
                    // 1バイト文字
                    uCharCount += 1;
                }
                else if (unicode <= 0x7FF)
                {
                    // 2バイト文字
                    uCharCount += 2;
                }
                else if (unicode <= 0xFFFF)
                {
                    // 3バイト文字
                    uCharCount += 3;
                }
                else if (unicode <= 0x10FFFF)
                {
                    // 4バイト文字
                    uCharCount += 4;
                }

                unicode = ::FT_Get_Next_Char(face, unicode, &freeTypeGlyphIndex);
                ++uGlyphCount;
            }
            ++uCharCount;
        }

        // 文字バッファを作る
        HE::Char* pHeapFontAllChar =
            reinterpret_cast<HE::Char*>(HE_ALLOC_MEM_LAST(sizeof(HE::Char) * uCharCount, 0));
        ::memset(pHeapFontAllChar, 0, sizeof(HE::Char) * uCharCount);

        // グリフの数分データを生成
        FontMaterial::Glyph* pGlyphs = reinterpret_cast<FontMaterial::Glyph*>(
            HE_ALLOC_MEM_LAST(sizeof(FontMaterial::Glyph) * uGlyphCount, 0));

        Core::Common::StringBase s(pHeapFontAllChar, uCharCount, TRUE);
        {
            auto unicode = ::FT_Get_First_Char(face, &freeTypeGlyphIndex);

            HE::Uint32 uGlyphIndex = 0;
            while (freeTypeGlyphIndex != 0)
            {
                FontMaterial::Glyph* pGlyph = &pGlyphs[uGlyphIndex];
                pGlyph->uUnicode            = unicode;

                if (unicode <= 0x7F)
                {
                    // 1バイト文字
                    s += (HE::Char)(unicode);
                }
                else if (unicode <= 0x7FF)
                {
                    // 2バイト文字
                    s += (HE::Char)(0xC0 | ((unicode >> 6) & 0x1F));
                    s += (HE::Char)(0x80 | (unicode & 0x3F));
                }
                else if (unicode <= 0xFFFF)
                {
                    // 3バイト文字
                    s += (HE::Char)(0xE0 | ((unicode >> 12) & 0x0F));
                    s += (HE::Char)(0x80 | ((unicode >> 6) & 0x3F));
                    s += (HE::Char)(0x80 | (unicode & 0x3F));
                }
                else if (unicode <= 0x10FFFF)
                {
                    // 4バイト文字
                    s += (HE::Char)(0xF0 | ((unicode >> 18) & 0x07));
                    s += (HE::Char)(0x80 | ((unicode >> 12) & 0x3F));
                    s += (HE::Char)(0x80 | ((unicode >> 6) & 0x3F));
                    s += (HE::Char)(0x80 | (unicode & 0x3F));
                }

                unicode = ::FT_Get_Next_Char(face, unicode, &freeTypeGlyphIndex);
                ++uGlyphIndex;
            }
            s += HE_STR_TEXT('\0');
        }

        buildData.szAllChar   = pHeapFontAllChar;
        buildData.pGlyphs     = pGlyphs;
        buildData.uGlyphCount = uGlyphCount;
        buildData.uCharCount  = uCharCount;

        ::FT_Done_Face(face);
        ::FT_Done_FreeType(library);

        return buildData;
    }

    static void _ReleaseFontBuildData(FontBuildData& in_rFontBuildData)
    {
        HE_SAFE_DELETE_MEM(in_rFontBuildData.pGlyphs);
        HE_SAFE_DELETE_MEM(in_rFontBuildData.szAllChar);
    }

    Font::Font(PlatformSDL2::PlatformSDL2Module* in_pModule) : _pModule(in_pModule)
    {
    }

    void Font::VRelease()
    {
        this->VUnload();
    }

    HE::Bool Font::VLoad(const HE::Uint32 in_uFontSize,
                         std::initializer_list<Core::File::Path> in_aPath)
    {
        // ttf,vert,fragファイルのパスが入っている
        HE_ASSERT(in_aPath.size() == 3);

        auto filePathItr = in_aPath.begin();
        // ttfファイルロード
        auto [pFontBinary, uFontBinarySize] = this->_pModule->VFile()->VLoadBinary(*filePathItr);
        HE_ASSERT_RETURN_VALUE(NullHandle, pFontBinary);
        ++filePathItr;

        // 頂点シェーダーをロード
        auto [pFontMatVertShaderText, uFontMatVerShaderSize] =
            this->_pModule->VFile()->VLoadText(*filePathItr);
        if (pFontMatVertShaderText == NULL)
        {
            HE_SAFE_DELETE_MEM(pFontBinary);
            return FALSE;
        }
        ++filePathItr;

        // ピクセルシェーダーをロード
        auto [pFontMatFragShaderText, uFontMatFragShaderSize] =
            this->_pModule->VFile()->VLoadText(*filePathItr);
        if (pFontMatFragShaderText == NULL)
        {
            HE_SAFE_DELETE_MEM(pFontBinary);
            HE_SAFE_DELETE_MEM(pFontMatVertShaderText);
            return FALSE;
        }

        // TODO: 書き込む文字列を作る
        // TODO:
        // フォントの文字列を作るためにヒープ確報しているので使い終わったら解放しないとだめ
        auto fontBuildData = _CreateFontBuildData(pFontBinary, uFontBinarySize);
        Core::Common::StringBase s(fontBuildData.szAllChar, fontBuildData.uCharCount, TRUE);

        if (s.Empty())
        {
            _ReleaseFontBuildData(fontBuildData);

            HE_SAFE_DELETE_MEM(pFontMatFragShaderText);
            HE_SAFE_DELETE_MEM(pFontMatVertShaderText);
            HE_SAFE_DELETE_MEM(pFontBinary);

            return FALSE;
        }

        // TODO: SDL2_ttfのフォントデータを作成
        SDL_RWops* pRWops = ::SDL_RWFromMem(pFontBinary, uFontBinarySize);
        if (pRWops == NULL)
        {
            ::SDL_FreeRW(reinterpret_cast<SDL_RWops*>(pRWops));
            _ReleaseFontBuildData(fontBuildData);

            HE_SAFE_DELETE_MEM(pFontMatFragShaderText);
            HE_SAFE_DELETE_MEM(pFontMatVertShaderText);
            HE_SAFE_DELETE_MEM(pFontBinary);

            return FALSE;
        }

        // フォント色
        constexpr SDL_Color sdlColor{255, 255, 255, 255};

        // フォントのテクスチャと文字のUV情報を作成
        {
            // 再利用のためにポインタをリセット
            pRWops->seek(pRWops, 0, RW_SEEK_SET);

            Core::Common::Handle surfaceHandle;
            HE::Uint32 uFontSize = in_uFontSize;
            auto pFont           = ::TTF_OpenFontRW(pRWops, 0, uFontSize);
            if (pFont)
            {
                // TODO: フォントのサーフェイス作成
                SDL_Surface* pSurf = TTF_RenderUTF8_Blended(pFont, s.Str(), sdlColor);
                if (pSurf)
                {
                    // TODO: 文字列をテクスチャーへ書き込む
                    // TODO: テクスチャだけ差し替えはあるか？
                    this->_spTex = HE_MAKE_CUSTOM_SHARED_PTR((TextureSurface), GL_TEXTURE_2D,
                                                             pSurf->pixels, pSurf->w, pSurf->h);

                    // TODO: フォントマテリアルを作成
                    this->_spMat = HE_MAKE_CUSTOM_SHARED_PTR((FontMaterial));
                    this->_spMat->LoadShader(pFontMatVertShaderText, pFontMatFragShaderText);

                    // TODO: テクスチャを作ったので解放
                    SDL_FreeSurface(pSurf);
                }

                ::TTF_CloseFont(pFont);
            }
        }

        ::SDL_FreeRW(reinterpret_cast<SDL_RWops*>(pRWops));

        _ReleaseFontBuildData(fontBuildData);

        HE_SAFE_DELETE_MEM(pFontMatFragShaderText);
        HE_SAFE_DELETE_MEM(pFontMatVertShaderText);
        HE_SAFE_DELETE_MEM(pFontBinary);

        return TRUE;
    }

    void Font::VUnload()
    {
        if (this->_spMat) this->_spMat->VRelease();
        if (this->_spTex) this->_spTex->Release();

        this->_spMat.reset();
        this->_spTex.reset();
    }
}  // namespace PlatformSDL2
