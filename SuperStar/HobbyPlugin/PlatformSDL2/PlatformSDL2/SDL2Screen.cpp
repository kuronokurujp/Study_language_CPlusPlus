#include "SDL2Screen.h"

// #include <algorithm>
#include "./../PlatformSDL2Module.h"
#include "./SDL2Font.h"
#include "./Screen/Draw/Material.h"
#include "./Screen/Draw/Mesh.h"
#include "./Screen/Draw/Texture.h"
#include "./Screen/Scene.h"
#include "./Screen/ViewPort.h"
#include "./Screen/Window.h"
#include "Engine/Common/PoolManager.h"
#include "GL/glew.h"
#include "SDL2/SDL.h"

namespace PlatformSDL2
{
    /*
        using MaterialPool = Core::Common::RuntimePoolManager<Material>;
        using MeshPool     = Core::Common::RuntimePoolManager<Mesh>;
        using TexturePool  = Core::Common::RuntimePoolManager<TextureBase>;

        static Core::Memory::SharedPtr<MaterialPool> s_poolMaterial;
        static Core::Memory::SharedPtr<MeshPool> s_poolMesh;
        static Core::Memory::SharedPtr<TexturePool> s_poolTexture;
        */

    static SDL_Window* s_pDummyWindow    = NULL;
    static SDL_GLContext s_pShareContext = NULL;

    Screen::Screen(PlatformSDL2::PlatformSDL2Module* in_pSDL2Module)
    {
        HE_ASSERT(in_pSDL2Module);

        this->_pSDL2Module = in_pSDL2Module;

        /*
                // TODO: 確保した数値は仮
                s_poolMaterial = HE_MAKE_CUSTOM_SHARED_PTR((MaterialPool));
                s_poolMaterial->ReservePool(1024);

                s_poolMesh = HE_MAKE_CUSTOM_SHARED_PTR((MeshPool));
                s_poolMesh->ReservePool(1024);

                s_poolTexture = HE_MAKE_CUSTOM_SHARED_PTR((TexturePool));
                s_poolTexture->ReservePool(1024);
                */

        // ダミーのウィンドウとコンテキスト生成
        // なぜこうしているのか？
        // 利用するウィンドウを生成する前にOpenGLの命令が呼ぶケースがあるから
        // 利用するウィンドウが生成されたら消す
        s_pDummyWindow  = SDL_CreateWindow("", 0, 0, 0, 0, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
        s_pShareContext = SDL_GL_CreateContext(reinterpret_cast<SDL_Window*>(s_pDummyWindow));
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
        }

        // TODO: フォント用のメッシュは常に使うので常駐
        auto pFontMesh = HE_NEW_MEM(Mesh, 0)();
        pFontMesh->Init();

        this->_pFontMesh = pFontMesh;
    }

    void Screen::VRelease()
    {
        auto pFontMesh = reinterpret_cast<Mesh*>(this->_pFontMesh);
        pFontMesh->Release();
        HE_SAFE_DELETE_MEM(pFontMesh);

        /*
            // 確保したマテリアルを解放
            {
                s_poolMaterial->ReleasePool([](Material* in_pMat) { in_pMat->VRelease(); });
                s_poolMaterial.reset();
            }

            // 確保したメッシュを解放
            s_poolMesh->ReleasePool();
            s_poolMesh.reset();

            // 確保したテクスチャを解放
            s_poolTexture->ReleasePool([](TextureBase* in_pTex) { in_pTex->Release(); });
            s_poolTexture.reset();
            */
    }

    Core::Memory::UniquePtr<Platform::WindowStrategy> Screen::VCreateWindowStrategy(
        const Platform::WindowConfig& in_rConfig)
    {
        auto spSt = HE_MAKE_CUSTOM_UNIQUE_PTR((SDL2WindowStrategy), in_rConfig,
                                              static_cast<void*>(s_pShareContext));

        if (s_pDummyWindow)
        {
            ::SDL_DestroyWindow(s_pDummyWindow);
            s_pDummyWindow = NULL;
        }

        return std::move(spSt);
    }

    Core::Memory::UniquePtr<Platform::ViewPortStrategy> Screen::VCreateViewPortStrategy(
        const Platform::ViewPortConfig& in_rConfig)
    {
        auto spSt = HE_MAKE_CUSTOM_UNIQUE_PTR((SDL2ViewPortStrategy), in_rConfig);

        return std::move(spSt);
    }

    Core::Memory::UniquePtr<Platform::SceneStrategyInterface> Screen::VCreateSceneUIStrategy()
    {
        auto spSt = HE_MAKE_CUSTOM_UNIQUE_PTR((SDL2SceneStrategyUI));
        return std::move(spSt);
    }

    Core::Memory::UniquePtr<Platform::SceneStrategyInterface> Screen::VCreateScene2DStrategy()
    {
        auto spSt = HE_MAKE_CUSTOM_UNIQUE_PTR((SDL2SceneStrategy2D));
        return std::move(spSt);
    }

    void Screen::VCls(const HE::Uint32 in_uR, const HE::Uint32 in_uG, const HE::Uint32 in_uB)
    {
        HE::Float32 fR = static_cast<HE::Float32>(in_uR) / 255.0f;
        HE::Float32 fG = static_cast<HE::Float32>(in_uG) / 255.0f;
        HE::Float32 fB = static_cast<HE::Float32>(in_uB) / 255.0f;

        // 色は0 - 1.0
        ::glClearColor(fR, fG, fB, 1.0f);
    }

    void Screen::VDrawText2D(const Platform::ViewPortConfig& in_rViewConfig,
                             const Core::Math::Vector2& in_rPos, const HE::Char* in_szText,
                             const HE::Uint32 in_uTextSize,
                             const Core::Math::Rect2::EAnchor in_eAnchor,
                             const Core::Math::Color& in_rColor)
    {
        // TODO: アンカー処理がまだ

        auto pFontInterface = this->_pSDL2Module->VFont();
        Font* pFont         = reinterpret_cast<Font*>(pFontInterface.get());
        HE_ASSERT_RETURN(pFont);

        // フォントのマテリアルを取得
        auto pFontMat = pFont->GetMaterial();
        HE_ASSERT_RETURN(pFontMat);

        // フォントのテクスチャを取得
        auto pFontTex = pFont->GetTexture();
        HE_ASSERT_RETURN(pFontTex);

        // フォントの頂点データ構造体
        struct Vertex
        {
            // 頂点の位置
            Core::Math::Vector3 position;
            // UV座標
            Core::Math::Vector2 uv;
        };

        // TODO: 動的用を作ったら差し替える
        static std::vector<Vertex> s_vertices;
        s_vertices.clear();

        // 文字列に合わせて頂点配列を作り描画
        Mesh* pMesh = reinterpret_cast<Mesh*>(this->_pFontMesh);
        {
            // インデックスオフセット
            HE::Uint32 uIndexOffset = 0;
            // メッシュのサイズ
            constexpr HE::Float32 fMeshWidth  = 1.0f;
            constexpr HE::Float32 fMeshHeight = 1.0f;

            HE::Float32 fSX = 0;
            HE::Float32 fSY = 0;
            HE::Float32 fX  = fSX;
            HE::Float32 fY  = fSY;

            Core::Common::g_szTempFixedString1024 = in_szText;
            auto itrEnd                           = Core::Common::g_szTempFixedString1024.End();
            for (auto itr = Core::Common::g_szTempFixedString1024.Begin(); itr != itrEnd; ++itr)
            {
                if ((*itr)[0] == HE_STR_TEXT('\n'))
                {
                    // 改行したら左端に戻す
                    fX = fSX;
                    // 1行分下にずらす
                    fY -= fMeshHeight;

                    // 改行時にダミー頂点を挿入してつながりをリセット
                    // 前の行の最後の頂点をコピー
                    s_vertices.push_back(s_vertices.back());
                    // 新しい行の左下
                    s_vertices.push_back({Core::Math::Vector3(fX, fY - fMeshHeight, 0.0f),
                                          // UVは仮の値
                                          Core::Math::Vector2(0.0f, 0.0f)});
                    continue;
                }

                // UTF8からunicodeに変える
                const HE::Uint32 uUnicode = Core::Common::GetUTF8CharToUnicode(*itr);

                // グリフのUV値を元に設定
                auto pGlyph = pFontMat->GetGlyph(uUnicode);

                // 頂点データ：四角形をGL_TRIANGLE_STRIPに対応させる (時計回り)
                // 左下
                s_vertices.push_back({Core::Math::Vector3(fX, fY - fMeshHeight, 0.0f),
                                      Core::Math::Vector2(pGlyph->_fTexSU, pGlyph->_fTexEV)});

                // 左上
                s_vertices.push_back({Core::Math::Vector3(fX, fY, 0.0f),
                                      Core::Math::Vector2(pGlyph->_fTexSU, pGlyph->_fTexSV)});
                // 右下
                s_vertices.push_back({Core::Math::Vector3(fX + fMeshWidth, fY - fMeshHeight, 0.0f),
                                      Core::Math::Vector2(pGlyph->_fTexEU, pGlyph->_fTexEV)});
                // 右上
                s_vertices.push_back({Core::Math::Vector3(fX + fMeshWidth, fY, 0.0f),
                                      Core::Math::Vector2(pGlyph->_fTexEU, pGlyph->_fTexSV)});

                /*
                                // 左下
                                s_vertices.push_back(
                                    {Core::Math::Vector3(-1.f, -1.f, 0.0f), Core::Math::Vector2(0,
                   1)});
                                // 右下
                                s_vertices.push_back(
                                    {Core::Math::Vector3(1.f, -1.f, 0.0f), Core::Math::Vector2(1,
                   1)});
                                // 右上
                                s_vertices.push_back(
                                    {Core::Math::Vector3(1.f, 1.f, 0.0f), Core::Math::Vector2(1,
                   0)});
                                // 左上
                                s_vertices.push_back(
                                    {Core::Math::Vector3(-1.f, 1.f, 0.0f), Core::Math::Vector2(0,
                   0)});

                                // インデックスデータ：2つの三角形 (0, 1, 2) と (2, 3, 0)
                                s_indices.push_back(0);
                                s_indices.push_back(1);
                                s_indices.push_back(2);
                                s_indices.push_back(2);
                                s_indices.push_back(3);
                                s_indices.push_back(0);
                                */

                // 次の文字の位置に移動
                fX += fMeshWidth;
                // 1文字あたり4つの頂点
                uIndexOffset += 4;
            }

            //  頂点など描画に必要な情報をバインド
            {
                Mesh::IndexData bindMeshIndexData;
                {
                    bindMeshIndexData._pIndices      = NULL;
                    bindMeshIndexData._uIndicesCount = 0;
                }

                Mesh::VertexData bindMeshVertexData;
                {
                    bindMeshVertexData._pVertices      = s_vertices.data();
                    bindMeshVertexData._uVerticesCount = s_vertices.size();
                    bindMeshVertexData._uVertSize      = sizeof(Vertex);
                    bindMeshVertexData._aBindLayout    = {
                        Mesh::LayoutData(3, sizeof(Vertex::position)),
                        Mesh::LayoutData(2, sizeof(Vertex::uv)),
                    };
                }

                pMesh->WriteDrawData(bindMeshVertexData, bindMeshIndexData);
            }
        }

        // 機能有効
        {
            // テクスチャを有効
            pFontTex->Enable();
            // マテリアルを有効
            pFontMat->Enable();
        }

        // 射影はシンプルなのを利用
        {
            Core::Math::Matrix4 viewProj;
            Core::Math::Matrix4::OutputSimpleViewProj(&viewProj,
                                                      static_cast<HE::Float32>(
                                                          in_rViewConfig._uWidth),
                                                      static_cast<HE::Float32>(
                                                          in_rViewConfig._uHeight));
            pFontMat->SetPropertyMatrix("uViewProj", viewProj);
        }

        // 座標を設定
        {
            Core::Math::Matrix4 scaleMat;
            Core::Math::Matrix4::OutputScale(&scaleMat, in_uTextSize, in_uTextSize, 1.0f);

            Core::Math::Matrix4 transMat;
            // 原点を画面の左上にする
            Core::Math::Matrix4::OutputTranslation(
                &transMat,
                Core::Math::Vector3(in_rPos._fX - (in_rViewConfig._uWidth >> 1),
                                    -in_rPos._fY + (in_rViewConfig._uHeight >> 1), 0.0f));

            Core::Math::Matrix4 world;
            world.Set(scaleMat);

            world.Mul(transMat);
            pFontMat->SetPropertyMatrix("uWorldTransform", world);
        }

        // フォントテクスチャ設定
        {
            pFontMat->SetPropertyTexture("uTexture", pFontTex.get());
        }

        // すべての準備が整ったので描画
        pMesh->DrawByVertexOnly(GL_TRIANGLE_STRIP);

        // 機能無効
        {
            // マテリアルを無効
            pFontMat->Disable();
            // テクスチャを無効
            pFontTex->Disable();
        }
        pMesh->FreeDrawData();
    }
}  // namespace PlatformSDL2
