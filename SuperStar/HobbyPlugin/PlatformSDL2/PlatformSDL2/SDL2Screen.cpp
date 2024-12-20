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
            GLenum glewResult = glewInit();
            if (glewResult != GLEW_OK)
            {
                HE_ASSERT(0);
                HE_LOG_LINE(HE_STR_TEXT("Unable to initialize GLEW: %s"),
                            ::glewGetErrorString(glewResult));
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

    void Screen::VDrawText2D(const Core::Math::Vector2& in_rPos, const HE::Char* in_szText,
                             const Core::Math::Rect2::EAnchor in_eAnchor,
                             const Core::Math::Color& in_rColor)
    {
        auto pFontInterface = this->_pSDL2Module->VFont();
        Font* pFont         = reinterpret_cast<Font*>(pFontInterface.get());

        // TODO: フォントのマテリアルを取得
        auto pFontMat = pFont->GetMaterial();
        HE_ASSERT(pFontMat);
        if (pFontMat == NULL) return;

        // TODO: フォントのテクスチャを取得
        auto pFontTex = pFont->GetTexture();
        HE_ASSERT(pFontTex);
        if (pFontTex == NULL) return;

        // TODO: 文字列に合わせて頂点配列を作り描画
        Mesh* pMesh = reinterpret_cast<Mesh*>(this->_pFontMesh);
        {
            // フォントの頂点データ構造体
            struct Vertex
            {
                // 頂点の位置
                Core::Math::Vector3 position;
                // UV座標
                Core::Math::Vector2 uv;
            };

            // TODO: 試す
            // TODO: 頂点やインデックスは数を固定にできないの
            // TODO: 動的用を作ったら差し替える
            static std::vector<Vertex> s_vertices;
            static std::vector<HE::Uint32> s_indices;
            s_vertices.clear();
            s_indices.clear();

            // インデックスオフセット
            GLuint indexOffset = 0;
            // TODO: 適当、後で整理
            HE::Float32 charWidth  = 1.0f;
            HE::Float32 charHeight = 1.0f;

            HE::Float32 sx = -charWidth * 0.5f;
            HE::Float32 x  = sx;
            HE::Float32 sy = -charHeight * 0.5f;
            HE::Float32 y  = sy;

            Core::Common::g_szTempFixedString1024 = in_szText;
            auto itrEnd                           = Core::Common::g_szTempFixedString1024.End();
            for (auto itr = Core::Common::g_szTempFixedString1024.Begin(); itr != itrEnd; ++itr)
            {
                if ((*itr)[0] == HE_STR_TEXT('\n'))
                {
                    // 改行したら左端に戻す
                    x = sx;

                    // 1行分下にずらす
                    // TODO: ずらす量はテスト
                    y -= charHeight;
                    continue;
                }

                // TODO: UTF8からunicodeに変える
                HE::Uint32 uUnicode = 0;
                // TODO: まとめた方がいい？
                {
                    HE::UTF8 c     = (*itr)[0];
                    HE::UTF8* utf8 = &(*itr)[0];
                    if ((c & 0x80) == 0x00)
                    {  // 1バイト文字 (ASCII)
                        uUnicode = c;
                    }
                    else if ((c & 0xE0) == 0xC0)
                    {  // 2バイト文字
                        uUnicode = ((c & 0x1F) << 6) | (utf8[1] & 0x3F);
                    }
                    else if ((c & 0xF0) == 0xE0)
                    {  // 3バイト文字
                        uUnicode = ((c & 0x0F) << 12) | ((utf8[1] & 0x3F) << 6) | (utf8[2] & 0x3F);
                    }
                    else if ((c & 0xF8) == 0xF0)
                    {  // 4バイト文字
                        uUnicode = ((c & 0x07) << 18) | ((utf8[1] & 0x3F) << 12) |
                                   ((utf8[2] & 0x3F) << 6) | (utf8[3] & 0x3F);
                    }
                    else
                    {
                        // TODO: エラーにする？
                    }
                }

                // TODO: グリフのUV値を元に設定
                auto pGlpyh = pFontMat->GetGlyph(uUnicode);

                // 頂点データ：四角形の2つの三角形として格納 (時計回り)
                // 左下
                s_vertices.push_back({Core::Math::Vector3(x, y, 0.0f),
                                      Core::Math::Vector2(pGlpyh->_fTexSU, pGlpyh->_fTexEV)});
                // 右下
                s_vertices.push_back({Core::Math::Vector3(x + charWidth, y, 0.0f),
                                      Core::Math::Vector2(pGlpyh->_fTexEU, pGlpyh->_fTexEV)});
                // 右上
                s_vertices.push_back({Core::Math::Vector3(x + charWidth, y + charHeight, 0.0f),
                                      Core::Math::Vector2(pGlpyh->_fTexEU, pGlpyh->_fTexSV)});
                // 左上
                s_vertices.push_back({Core::Math::Vector3(x, y + charHeight, 0.0f),
                                      Core::Math::Vector2(pGlpyh->_fTexSU, pGlpyh->_fTexSV)});

                // インデックスデータ：2つの三角形 (0, 1, 2) と (2, 3, 0)
                s_indices.push_back(indexOffset + 0);
                s_indices.push_back(indexOffset + 1);
                s_indices.push_back(indexOffset + 2);
                s_indices.push_back(indexOffset + 2);
                s_indices.push_back(indexOffset + 3);
                s_indices.push_back(indexOffset + 0);

                // 次の文字の位置に移動
                x += charWidth;
                // 1文字あたり4つの頂点
                indexOffset += 4;
            }

            //  頂点など描画に必要な情報をバインド
            {
                Mesh::BindMeshIndexData bindMeshIndexData;
                {
                    bindMeshIndexData._pIndices      = s_indices.data();
                    bindMeshIndexData._uIndicesCount = s_indices.size();
                }

                Mesh::BindMeshVertexData bindMeshVertexData;
                {
                    bindMeshVertexData._pVertices      = s_vertices.data();
                    bindMeshVertexData._uVerticesCount = s_vertices.size();
                    bindMeshVertexData._aBindLayout    = {
                        Mesh::BindMeshLayoutData(3, sizeof(Vertex::position)),
                        Mesh::BindMeshLayoutData(2, sizeof(Vertex::uv)),
                    };
                }

                pMesh->BindMeshData(bindMeshVertexData, bindMeshIndexData);
            }
        }

        // TODO: 機能有効
        {
            // TODO: テクスチャを有効
            // pFontTex->VEnable();
            // TODO: マテリアルを有効
            pFontMat->Enable();
        }

#if 0
        // TODO: 画面の縦横を設定
        // プロパティ名固定は大丈夫なのか?
        {
            Core::Math::Matrix4 viewProj;
            Core::Math::Matrix4::OutputSimpleViewProj(&viewProj,
                                                      static_cast<HE::Float32>(
                                                          pViewPortConfig->uWidth),
                                                      static_cast<HE::Float32>(
                                                          pViewPortConfig->uHeight));
            pFontMat->SetPropertyMatrix("uViewProj", viewProj);
        }

        // TODO: 座標を設定
        {
            Core::Math::Matrix4 scaleMat;
            Core::Math::Matrix4::OutputScale(&scaleMat, 32.0f, 32.0f, 1.0f);

            Core::Math::Matrix4 transMat;
            Core::Math::Matrix4::OutputTranslation(&transMat,
                                                   Core::Math::Vector3(in_rPos._fX, in_rPos._fY,
                                                                       0.0f));

            Core::Math::Matrix4 world;
            world.Set(scaleMat);

            world.Mul(transMat);
            // プロパティ名固定は大丈夫なのか?
            pFontMat->SetPropertyMatrix("uWorldTransform", world);
        }
#endif
        // TODO: バインドやプロパティ設定などすべての準備が整ったので描画
        pMesh->Draw();

        // TODO: 機能無効
        {
            // TODO: マテリアルを無効
            pFontMat->Disable();
            // TODO: テクスチャを無効
            // pFontTex->VDisable();
        }
    }
}  // namespace PlatformSDL2
