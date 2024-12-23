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
    namespace Local
    {
        struct FontData
        {
            FT_Library _ftLib   = NULL;
            FT_Face _ftFace     = NULL;
            SDL_RWops* _pRWops  = NULL;
            TTF_Font* _pTTFFont = NULL;
        };

        constexpr HE::Uint32 aFontSize[] = {64};

        static FontData OpenFont(void* in_pFontBinary, const HE::Uint32 in_uFontDataSize,
                                 const HE::Uint32 in_uFontSize)
        {
            FT_Face face;
            FT_Library library;

            // FreeType ライブラリを初期化
            if (::FT_Init_FreeType(&library))
            {
                HE_LOG_LINE(HE_STR_TEXT("FreeTypeのライブラリ初期化に失敗"));
                return FontData{NULL, NULL, NULL, NULL};
            }

            // メモリ上のフォントデータを利用して Face を作成
            if (::FT_New_Memory_Face(library, reinterpret_cast<FT_Byte*>(in_pFontBinary),
                                     in_uFontDataSize, 0, &face))
            {
                HE_LOG_LINE(HE_STR_TEXT("FreeTypeのFace生成が失敗"));

                return FontData{library, NULL, NULL, NULL};
            }

            // TODO: SDL2_ttfのフォントデータを作成
            SDL_RWops* pRWops = ::SDL_RWFromMem(in_pFontBinary, in_uFontDataSize);
            if (pRWops == NULL)
            {
                return FontData{library, face, NULL, NULL};
            }

            pRWops->seek(pRWops, 0, RW_SEEK_SET);

            auto pFont = ::TTF_OpenFontRW(pRWops, 0, in_uFontSize);
            return FontData{library, face, pRWops, pFont};
        }

        static void CloseFont(FontData* in_pData)
        {
            if (in_pData->_pTTFFont) ::TTF_CloseFont(in_pData->_pTTFFont);

            if (in_pData->_pRWops) ::SDL_FreeRW(reinterpret_cast<SDL_RWops*>(in_pData->_pRWops));

            if (in_pData->_ftFace) ::FT_Done_Face(in_pData->_ftFace);

            if (in_pData->_ftLib) ::FT_Done_FreeType(in_pData->_ftLib);

            in_pData->_pRWops   = NULL;
            in_pData->_ftFace   = NULL;
            in_pData->_pTTFFont = NULL;
            in_pData->_ftLib    = NULL;
        }

        // テクスチャを構築し、グリフのUVを計算する
        static HE::Bool OutputTextureAndUV(SDL_Surface** out_ppSurface,
                                           FontMaterial::GlyphMap* out_pGlyphMap,
                                           const FontData& in_rData, const HE::Uint32 in_uTexW,
                                           const HE::Uint32 in_uTexH, const SDL_Color in_color)
        {
            // SDLサーフェイスを作成
            *out_ppSurface = ::SDL_CreateRGBSurface(0, in_uTexW, in_uTexH, 32,
                                                    0x00FF0000,   // Red mask
                                                    0x0000FF00,   // Green mask
                                                    0x000000FF,   // Blue mask
                                                    0xFF000000);  // Alpha mask
            if ((*out_ppSurface) == NULL)
            {
                HE_LOG_LINE(HE_STR_TEXT("Failed to create surface: %d"), ::SDL_GetError());
                return FALSE;
            }

            // フォント内の文字を列挙
            ::FT_UInt freeTypeGlyphIndex = 0;
            ::FT_ULong unicode           = 0;
            HE::Uint32 uCharCount        = 0;
            HE::Uint32 uGlyphCount       = 0;

            HE::Float32 fInvTexW = 1.0f / static_cast<HE::Float32>(in_uTexW);
            HE::Float32 fInvTexH = 1.0f / static_cast<HE::Float32>(in_uTexH);
            // まずどれくらい文字バッファサイズが必要かバッファ数を計算
            {
                unicode = ::FT_Get_First_Char(in_rData._ftFace, &freeTypeGlyphIndex);

                // グリフを並べるための現在の描画位置
                int x = 0, y = 0, rowHeight = 0;

                SDL_Surface* pGlyphSurf = NULL;
                while (freeTypeGlyphIndex != 0)
                {
                    pGlyphSurf = ::TTF_RenderGlyph_Blended(in_rData._pTTFFont, unicode, in_color);
                    if (pGlyphSurf == NULL)
                    {
                        HE_LOG_LINE(HE_STR_TEXT("Failed to render glyph: %d %d"), unicode,
                                    TTF_GetError());
                        continue;
                    }

                    // グリフが横幅を超えた場合は次の行に移動
                    if (in_uTexW <= (x + pGlyphSurf->w))
                    {
                        x = 0;
                        y += rowHeight;
                        HE_ASSERT(y < (*out_ppSurface)->h);

                        rowHeight = 0;
                    }

                    // グリフをサーフェイスにコピー
                    SDL_Rect destRect = {x, y, pGlyphSurf->w, pGlyphSurf->h};
                    SDL_BlitSurface(pGlyphSurf, nullptr, *out_ppSurface, &destRect);

                    // グリフのUVを計算して保存
                    (*out_pGlyphMap)[unicode] =
                        {static_cast<HE::Float32>(x) * fInvTexW,
                         static_cast<HE::Float32>(x + pGlyphSurf->w) * fInvTexW,
                         static_cast<HE::Float32>(y) * fInvTexH,
                         static_cast<HE::Float32>(y + pGlyphSurf->h) * fInvTexH,
                         static_cast<HE::Uint32>(pGlyphSurf->w),
                         static_cast<HE::Uint32>(pGlyphSurf->h)};

                    // X座標を更新
                    x += pGlyphSurf->w;
                    rowHeight = HE_MAX(rowHeight, pGlyphSurf->h);

                    // サーフェイスを解放
                    ::SDL_FreeSurface(pGlyphSurf);

                    unicode = ::FT_Get_Next_Char(in_rData._ftFace, unicode, &freeTypeGlyphIndex);
                }
            }

            return TRUE;
        }
    }  // namespace Local

    Font::Font(PlatformSDL2::PlatformSDL2Module* in_pModule) : _pModule(in_pModule)
    {
    }

    void Font::VRelease()
    {
        this->VUnload();
    }

    HE::Bool Font::VLoad(const Platform::EFontSize in_eSizeType,
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

        HE::Uint32 uFontSize = Local::aFontSize[in_eSizeType];
        Local::FontData fontData;
        FontMaterial::GlyphMap mGlypMap;
        SDL_Surface* pFontSurf = NULL;

        fontData = Local::OpenFont(pFontBinary, uFontBinarySize, uFontSize);
        {
            // TODO: フォントサイズが高くなるとテクスチャ内に文字が収まらない
            // 動的にテクスチャサイズを変える必要がある
            HE::Uint32 uTexW = 2048;
            HE::Uint32 uTexH = 4096;

            Local::OutputTextureAndUV(&pFontSurf, &mGlypMap, fontData, uTexW, uTexH,
                                      {255, 255, 255, 255});

            // 文字列をテクスチャーへ書き込む
            this->_spTex = HE_MAKE_CUSTOM_SHARED_PTR((TextureSurface), GL_TEXTURE_2D, GL_TEXTURE0,
                                                     pFontSurf->pixels, uTexW, uTexH);
            // フォントマテリアルを作成
            this->_spMat = HE_MAKE_CUSTOM_SHARED_PTR((FontMaterial));
            this->_spMat->LoadShader(pFontMatVertShaderText, pFontMatFragShaderText);
            this->_spMat->CopyGlyphs(std::move(mGlypMap));

            Local::CloseFont(&fontData);
        }

        ::SDL_FreeSurface(pFontSurf);

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
