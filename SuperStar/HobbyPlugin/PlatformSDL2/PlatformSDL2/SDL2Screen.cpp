#include "SDL2Screen.h"

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
    static SDL_Window* s_pDummyWindow    = NULL;
    static SDL_GLContext s_pShareContext = NULL;

    Screen::Screen(PlatformSDL2::PlatformSDL2Module* in_pSDL2Module)
    {
        HE_ASSERT(in_pSDL2Module);

        this->_pSDL2Module = in_pSDL2Module;

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

        // フォント用のメッシュは常に使うので常駐
        {
            auto pFontMesh = HE_NEW_MEM(Mesh, 0)();
            pFontMesh->Init();
            this->_pFontMesh = pFontMesh;
        }

        // 2D矩形用のマテリアルを作成
        {
            auto p2DQuadMat = HE_NEW_MEM(Material, 0);

            // 描画処理でプロパティ名を直指定しているので外部ファイル化はしない
            // TODO: シェーダーを差し替える機能もいずれ考える
            // 頂点シェーダーのコード
            constexpr HE::UTF8* szVertShaderText =
                " \
            #version 330 \n \
            uniform mat4 uWorldTransform; \
            uniform mat4 uViewProj; \
            out vec2 fragTexCoord; \
            void main() \
            {\
                fragTexCoord = vec2(gl_VertexID % 2, gl_VertexID / 2); \
                vec4 pos     = vec4(fragTexCoord.x, -fragTexCoord.y, 0.0, 1.0);\
                gl_Position  = pos * uWorldTransform * uViewProj;\
            }";

            // ピクセルシェーダーのコード
            constexpr HE::UTF8* szFragShaderText =
                " \
            #version 330 \n \
            in vec2 fragTexCoord; \
            out vec4 outColor; \
            uniform sampler2D uTexture; \
            uniform vec4 uColor; \
            void main() \
            {\
                vec4 tex = texture2D(uTexture, fragTexCoord);\
                outColor = tex * uColor;\
            }";

            if (p2DQuadMat->LoadShader(szVertShaderText, szFragShaderText) == FALSE)
            {
                HE_ASSERT(FALSE && "2D矩形のシェーダーロードに失敗");
            }
            this->_p2DQuadMat = p2DQuadMat;
        }

        // 2D汎用幾何学用のマテリアルを作成
        {
            auto pMat = HE_NEW_MEM(Material, 0);

            // 描画処理でプロパティ名を直指定しているので外部ファイル化はしない
            // TODO: シェーダーを差し替える機能もいずれ考える
            // 頂点シェーダーのコード
            constexpr HE::UTF8* szVertShaderText =
                " \
            #version 330 \n \
            uniform mat4 uWorldTransform; \
            uniform mat4 uViewProj; \
            layout(location = 0) in vec3 inPosition; \
            layout(location = 1) in vec2 inTexCoord; \
            out vec2 fragTexCoord; \
            void main() \
            {\
                fragTexCoord = inTexCoord; \
                vec4 pos     = vec4(inPosition, 1.0);\
                gl_Position  = pos * uWorldTransform * uViewProj;\
            }";

            // ピクセルシェーダーのコード
            constexpr HE::UTF8* szFragShaderText =
                " \
            #version 330 \n \
            in vec2 fragTexCoord; \
            out vec4 outColor; \
            uniform sampler2D uTexture; \
            uniform vec4 uColor; \
            void main() \
            {\
                vec4 tex = texture2D(uTexture, fragTexCoord);\
                outColor = tex * uColor;\
            }";

            if (pMat->LoadShader(szVertShaderText, szFragShaderText) == FALSE)
            {
                HE_ASSERT(FALSE && "2D幾何学のシェーダーロードに失敗");
            }
            this->_p2DGeometoryMat = pMat;
        }

        // 2Dの矩形メッシュ作成
        {
            auto p2DQuadMesh = HE_NEW_MEM(Mesh, 0)();
            p2DQuadMesh->Init();

            //  頂点など描画に必要な情報をバインド
            {
                Mesh::IndexData bindMeshIndexData;
                {
                    bindMeshIndexData._pIndices      = NULL;
                    bindMeshIndexData._uIndicesCount = 0;
                }

                Mesh::VertexData bindMeshVertexData;
                {
                    bindMeshVertexData._pVertices      = NULL;
                    bindMeshVertexData._uVerticesCount = 0;
                    bindMeshVertexData._uVertSize      = 0;
                    bindMeshVertexData._aBindLayout    = {};
                }

                p2DQuadMesh->WriteDrawData(bindMeshVertexData, bindMeshIndexData);
            }
            this->_p2DQuadMesh = p2DQuadMesh;
        }

        // 2D円のメッシュ構築
        {
            auto p2DCircleMesh = HE_NEW_MEM(Mesh, 0)();
            struct CircleVertex
            {
                // 頂点の位置
                Core::Math::Vector3 position;
                // UV座標
                Core::Math::Vector2 uv;
            };

            std::vector<CircleVertex> vertices;
            // 以下の値を上げると円の描画精度が上がるが,メモリ消費量が上がる
            constexpr HE::Uint32 uSides = 256;

            // 円の縦横の辺の長さは1にする
            // 円の半径1だと円のサイズは2になるので0.5で半分にしている
            // 円の原点左上にしている
            const Core::Math::Vector3 cx(0.5f, -0.5f, 0.0f);

            Core::Math::Vector3 vec(0.0f, 0.0f, 0.0f);
            Core::Math::Vector2 uv(0.5f, 0.5f);

            vertices.push_back({cx, uv});
            for (auto i = 0; i <= uSides; ++i)
            {
                const auto fRate = Core::Math::Range0to1(i, uSides);
                const auto fRad  = Core::Math::fTwoPI * fRate;
                const auto fSin  = Core::Math::Sin(fRad);
                const auto fCos  = Core::Math::Cos(fRad);

                vec._fX = 0.5f + fCos * 0.5f;
                vec._fY = -0.5f + fSin * 0.5f;
                vec._fZ = 0.0f;

                uv._fX = 0.5f + 0.5f * fCos;
                uv._fY = 0.5f + 0.5f * fSin;

                vertices.push_back({vec, uv});
            }
            p2DCircleMesh->Init();

            //  頂点など描画に必要な情報をバインド
            {
                Mesh::IndexData bindMeshIndexData;
                {
                    bindMeshIndexData._pIndices      = NULL;
                    bindMeshIndexData._uIndicesCount = 0;
                }

                Mesh::VertexData bindMeshVertexData;
                {
                    bindMeshVertexData._pVertices      = vertices.data();
                    bindMeshVertexData._uVerticesCount = vertices.size();
                    bindMeshVertexData._uVertSize      = sizeof(CircleVertex);
                    bindMeshVertexData._aBindLayout    = {
                        Mesh::LayoutData(3, sizeof(CircleVertex::position)),
                        Mesh::LayoutData(2, sizeof(CircleVertex::uv)),
                    };
                }

                p2DCircleMesh->WriteDrawData(bindMeshVertexData, bindMeshIndexData);
            }

            this->_p2DCircleMesh = p2DCircleMesh;
        }

        // 2Dの三角形メッシュ
        {
            auto pMesh = HE_NEW_MEM(Mesh, 0)();
            pMesh->Init();

            struct LocalVertex
            {
                // 頂点の位置
                Core::Math::Vector3 position;
                // UV座標
                Core::Math::Vector2 uv;
            };

            std::vector<LocalVertex> vertices;

            vertices.push_back(
                {Core::Math::Vector3(0.5f, 0.0f, 0.0f), Core::Math::Vector2(0.5f, 0.f)});
            vertices.push_back(
                {Core::Math::Vector3(0.0f, -1.0f, 0.0f), Core::Math::Vector2(0.f, 1.f)});
            vertices.push_back(
                {Core::Math::Vector3(1.0f, -1.0f, 0.0f), Core::Math::Vector2(1.f, 1.f)});

            //  頂点など描画に必要な情報をバインド
            {
                Mesh::IndexData bindMeshIndexData;
                {
                    bindMeshIndexData._pIndices      = NULL;
                    bindMeshIndexData._uIndicesCount = 0;
                }

                Mesh::VertexData bindMeshVertexData;
                {
                    bindMeshVertexData._pVertices      = vertices.data();
                    bindMeshVertexData._uVerticesCount = vertices.size();
                    bindMeshVertexData._uVertSize      = sizeof(LocalVertex);
                    bindMeshVertexData._aBindLayout    = {
                        Mesh::LayoutData(3, sizeof(LocalVertex::position)),
                        Mesh::LayoutData(2, sizeof(LocalVertex::uv)),
                    };
                }

                pMesh->WriteDrawData(bindMeshVertexData, bindMeshIndexData);
            }
            this->_p2DTriangleMesh = pMesh;
        }

        // 白色のテクスチャ作成
        {
            HE::Uint32 aPixel[1] = {Core::Math::RGB::White.c};
            auto pWhiteTex =
                HE_NEW_MEM(TextureSurface, 0)(GL_TEXTURE_2D, GL_TEXTURE0, aPixel, 1, 1);
            this->_pWhiteTex = pWhiteTex;
        }
    }

    void Screen::VRelease()
    {
        {
            auto pWhiteTex = reinterpret_cast<TextureSurface*>(this->_pWhiteTex);
            pWhiteTex->Release();
            HE_SAFE_DELETE_MEM(pWhiteTex);
        }

        {
            auto pMesh = reinterpret_cast<Mesh*>(this->_p2DTriangleMesh);
            pMesh->Release();
            HE_SAFE_DELETE_MEM(this->_p2DTriangleMesh);
        }

        {
            auto p2DCircleMesh = reinterpret_cast<Mesh*>(this->_p2DCircleMesh);
            p2DCircleMesh->Release();
            HE_SAFE_DELETE_MEM(this->_p2DCircleMesh);
        }

        {
            auto pFontMesh = reinterpret_cast<Mesh*>(this->_pFontMesh);
            pFontMesh->Release();
            HE_SAFE_DELETE_MEM(this->_pFontMesh);
        }

        {
            auto p2DQuadMesh = reinterpret_cast<Mesh*>(this->_p2DQuadMesh);
            p2DQuadMesh->Release();
            HE_SAFE_DELETE_MEM(this->_p2DQuadMesh);
        }

        {
            auto pMat = reinterpret_cast<Material*>(this->_p2DGeometoryMat);
            pMat->VRelease();
            HE_SAFE_DELETE_MEM(this->_p2DGeometoryMat);
        }

        {
            auto p2DQuadMat = reinterpret_cast<Material*>(this->_p2DQuadMat);
            p2DQuadMat->VRelease();
            HE_SAFE_DELETE_MEM(this->_p2DQuadMat);
        }
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
        HE::Float32 fR = static_cast<HE::Float32>(in_uR) * Core::Math::fInvert255;
        HE::Float32 fG = static_cast<HE::Float32>(in_uG) * Core::Math::fInvert255;
        HE::Float32 fB = static_cast<HE::Float32>(in_uB) * Core::Math::fInvert255;

        // 色は0 - 1.0
        ::glClearColor(fR, fG, fB, 1.0f);
    }

    void Screen::V2DDrawText(const Platform::ViewPortConfig& in_rViewConfig,
                             const Core::Math::Vector2& in_rPos,
                             const Core::Math::EAnchor in_eAnchor, const HE::Char* in_szText,
                             const HE::Uint32 in_uTextSize, const HE::Uint32 in_uSpace,
                             const Core::Math::Color in_color)
    {
        if (in_uTextSize <= 0) return;

        Core::Common::g_szTempFixedString1024 = in_szText;
        if (Core::Common::g_szTempFixedString1024.Length() <= 0) return;

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

        HE::Float32 fObjectWidth  = 0.0f;
        HE::Float32 fObjectHeight = 0.0f;

        // 文字列に合わせて頂点配列を作り描画
        Mesh* pMesh = reinterpret_cast<Mesh*>(this->_pFontMesh);
        {
            // インデックスオフセット
            HE::Uint32 uIndexOffset = 0;
            // メッシュのサイズ
            constexpr HE::Float32 fMeshWidth  = 1.0f;
            constexpr HE::Float32 fMeshHeight = 1.0f;

            constexpr HE::Float32 fSX = 0;
            constexpr HE::Float32 fSY = 0;
            HE::Float32 _fX           = fSX;
            HE::Float32 _fY           = fSY;

            const HE::Float32 fSpace = (1.0f / static_cast<HE::Float32>(in_uTextSize)) *
                                       static_cast<HE::Float32>(in_uSpace);

            HE::Float32 fTmpWidth     = 0.0f;
            HE::Uint32 uVerticalCount = 0;
            fObjectHeight             = static_cast<HE::Float32>(in_uTextSize);

            auto itrEnd = Core::Common::g_szTempFixedString1024.End();
            for (auto itr = Core::Common::g_szTempFixedString1024.Begin(); itr != itrEnd; ++itr)
            {
                // UTF8からunicodeに変える
                const HE::Uint32 uUnicode = Core::Common::GetUTF8CharToUnicode(*itr);

                // グリフのUV値を元に設定
                auto pGlyph = pFontMat->GetGlyph(uUnicode);
                if (pGlyph == NULL) continue;

                if ((*itr)[0] == HE_STR_TEXT('\n'))
                {
                    // 改行したら左端に戻す
                    _fX = fSX;
                    // 1行分下にずらす
                    _fY -= fMeshHeight;

                    // 改行時にダミー頂点を挿入してつながりをリセット
                    // 前の行の最後の頂点をコピー
                    s_vertices.push_back(s_vertices.back());
                    // 新しい行の左下
                    s_vertices.push_back({Core::Math::Vector3(_fX, _fY - fMeshHeight, 0.0f),
                                          // UVは仮の値
                                          Core::Math::Vector2(0.0f, 0.0f)});

                    // 改行になったので横の最大サイズを更新
                    if (fObjectWidth < fTmpWidth) fObjectWidth = fTmpWidth;
                    fTmpWidth = 0.0f;

                    fObjectHeight += static_cast<HE::Float32>(in_uTextSize);
                    uVerticalCount = 0;

                    continue;
                }

                // 横の仮サイズを更新
                {
                    fTmpWidth += static_cast<HE::Float32>(in_uTextSize);

                    // スペース値も入れる
                    // 行の最初の1文字はスペースがないのでスペースサイズは不要
                    if (0 < uVerticalCount) fTmpWidth += static_cast<HE::Float32>(in_uSpace);
                }
                ++uVerticalCount;

                // 頂点データ：四角形をGL_TRIANGLE_STRIPに対応させる (時計回り)
                // 左下
                s_vertices.push_back({Core::Math::Vector3(_fX, _fY - fMeshHeight, 0.0f),
                                      Core::Math::Vector2(pGlyph->_fTexSU, pGlyph->_fTexEV)});

                // 左上
                s_vertices.push_back({Core::Math::Vector3(_fX, _fY, 0.0f),
                                      Core::Math::Vector2(pGlyph->_fTexSU, pGlyph->_fTexSV)});
                // 右下
                s_vertices.push_back(
                    {Core::Math::Vector3(_fX + fMeshWidth, _fY - fMeshHeight, 0.0f),
                     Core::Math::Vector2(pGlyph->_fTexEU, pGlyph->_fTexEV)});
                // 右上
                s_vertices.push_back({Core::Math::Vector3(_fX + fMeshWidth, _fY, 0.0f),
                                      Core::Math::Vector2(pGlyph->_fTexEU, pGlyph->_fTexSV)});

                // 次の文字の位置に移動
                _fX += fMeshWidth;
                // 1文字あたり4つの頂点
                uIndexOffset += 4;

                //  文字と文字の間にスペースを入れれるか試し
                if (0.0f < fSpace)
                {
                    // 頂点データ：四角形をGL_TRIANGLE_STRIPに対応させる (時計回り)
                    // 左下
                    s_vertices.push_back({Core::Math::Vector3(_fX, _fY - fMeshHeight, 0.0f),
                                          Core::Math::Vector2(-1.0f)});

                    // 左上
                    s_vertices.push_back(
                        {Core::Math::Vector3(_fX, _fY, 0.0f), Core::Math::Vector2(-1.0f)});
                    // 右下
                    s_vertices.push_back(
                        {Core::Math::Vector3(_fX + fSpace, _fY - fMeshHeight, 0.0f),
                         Core::Math::Vector2(-1.0f)});
                    // 右上
                    s_vertices.push_back(
                        {Core::Math::Vector3(_fX + fSpace, _fY, 0.0f), Core::Math::Vector2(-1.f)});

                    // 次の文字の位置に移動
                    _fX += fSpace;
                    // 1文字あたり4つの頂点
                    uIndexOffset += 4;
                }
            }

            // 頂点情報がない状態でテクスチャやマテリアルを有効にすると無効となってエラーになる
            // なので頂点情報がない場合は描画関連の処理せず終了させる
            if (s_vertices.size() <= 0) return;

            if (fObjectWidth < fTmpWidth)
            {
                fObjectWidth = fTmpWidth;
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
            switch (in_eAnchor)
            {
                case Core::Math::EAnchor_Left:
                {
                    fObjectWidth = fObjectHeight = 0.0f;
                    break;
                }
                case Core::Math::EAnchor_Center:
                {
                    fObjectWidth *= 0.5f;
                    fObjectHeight *= 0.5f;
                    break;
                }
            }

            Core::Math::Matrix4 scaleMat;
            Core::Math::Matrix4::OutputScale(&scaleMat, in_uTextSize, in_uTextSize, 1.0f);

            Core::Math::Matrix4 transMat;
            // 原点を画面の左上にする
            Core::Math::Matrix4::OutputTranslation(
                &transMat,
                Core::Math::Vector3((in_rPos._fX - fObjectWidth) - (in_rViewConfig._uWidth >> 1),
                                    -(in_rPos._fY - fObjectHeight) + (in_rViewConfig._uHeight >> 1),
                                    0.0f));

            Core::Math::Matrix4 world;
            world.Set(scaleMat);

            world.Mul(transMat);
            pFontMat->SetPropertyMatrix("uWorldTransform", world);
        }

        // フォントテクスチャ設定
        {
            pFontMat->SetPropertyTexture("uTexture", pFontTex.get());
        }

        // 色設定
        {
            const Core::Math::Color32& rC32 = in_color.c32;
            Core::Math::Vector4 _color(static_cast<HE::Float32>(rC32.r) * Core::Math::fInvert255,
                                       static_cast<HE::Float32>(rC32.g) * Core::Math::fInvert255,
                                       static_cast<HE::Float32>(rC32.b) * Core::Math::fInvert255,
                                       static_cast<HE::Float32>(rC32.a) * Core::Math::fInvert255);

            pFontMat->SetPropertyVector4("uColor", _color);
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

        // 描画内容は動的に変化するので描画データを破棄する
        pMesh->FreeDrawData();
    }

    void Screen::V2DDrawQuad(const Platform::ViewPortConfig& in_rViewConfig,
                             const Core::Math::Rect2& in_rRect2D, const Core::Math::Color in_color)
    {
        auto p2DQuadMat = reinterpret_cast<Material*>(this->_p2DQuadMat);
        p2DQuadMat->Enable();

        auto pWhiteTex = reinterpret_cast<TextureSurface*>(this->_pWhiteTex);
        pWhiteTex->Enable();
        {
            // 射影はシンプルなのを利用
            {
                Core::Math::Matrix4 viewProj;
                Core::Math::Matrix4::OutputSimpleViewProj(&viewProj,
                                                          static_cast<HE::Float32>(
                                                              in_rViewConfig._uWidth),
                                                          static_cast<HE::Float32>(
                                                              in_rViewConfig._uHeight));
                p2DQuadMat->SetPropertyMatrix("uViewProj", viewProj);
            }

            // 座標を設定
            {
                const auto fW = in_rRect2D.Width();
                const auto fH = in_rRect2D.Height();

                Core::Math::Matrix4 scaleMat;
                Core::Math::Matrix4::OutputScale(&scaleMat, fW, fH, 1.0f);

                Core::Math::Matrix4 transMat;
                const auto pos = in_rRect2D.Pos();
                Core::Math::Matrix4::OutputTranslation(
                    &transMat,
                    Core::Math::Vector3(pos._fX - (in_rViewConfig._uWidth >> 1),
                                        -(pos._fY) + (in_rViewConfig._uHeight >> 1), 0.0f));

                Core::Math::Matrix4 world;
                world.Set(scaleMat);

                world.Mul(transMat);
                p2DQuadMat->SetPropertyMatrix("uWorldTransform", world);
            }

            // 白色の1x1テクスチャを設定
            p2DQuadMat->SetPropertyTexture("uTexture", pWhiteTex);

            // 色設定
            {
                const Core::Math::Color32& rC32 = in_color.c32;
                Core::Math::Vector4
                    _color(static_cast<HE::Float32>(rC32.r) * Core::Math::fInvert255,
                           static_cast<HE::Float32>(rC32.g) * Core::Math::fInvert255,
                           static_cast<HE::Float32>(rC32.b) * Core::Math::fInvert255,
                           static_cast<HE::Float32>(rC32.a) * Core::Math::fInvert255);

                p2DQuadMat->SetPropertyVector4("uColor", _color);
            }

            auto p2DQuadMesh = reinterpret_cast<Mesh*>(this->_p2DQuadMesh);
            p2DQuadMesh->DrawByVertexOnly(GL_TRIANGLE_STRIP, 4);
        }
        pWhiteTex->Disable();
        p2DQuadMat->Disable();
    }

    void Screen::V2DDrawCircle(const Platform::ViewPortConfig& in_rViewConfig,
                               const Core::Math::Vector2& in_rPos,
                               const Core::Math::EAnchor in_eAchor, const HE::Float32 in_fSize,
                               const Core::Math::Color in_color)
    {
        auto pMat = reinterpret_cast<Material*>(this->_p2DGeometoryMat);
        pMat->Enable();

        auto pWhiteTex = reinterpret_cast<TextureSurface*>(this->_pWhiteTex);
        pWhiteTex->Enable();
        {
            // 射影はシンプルなのを利用
            {
                Core::Math::Matrix4 viewProj;
                Core::Math::Matrix4::OutputSimpleViewProj(&viewProj,
                                                          static_cast<HE::Float32>(
                                                              in_rViewConfig._uWidth),
                                                          static_cast<HE::Float32>(
                                                              in_rViewConfig._uHeight));
                pMat->SetPropertyMatrix("uViewProj", viewProj);
            }

            // 座標を設定
            {
                HE::Float32 fOffset = 0.0f;
                switch (in_eAchor)
                {
                    case Core::Math::EAnchor_Left:
                    {
                        break;
                    }

                    case Core::Math::EAnchor_Center:
                    {
                        fOffset = in_fSize * 0.5f;
                        break;
                    }
                }

                Core::Math::Matrix4 scaleMat;
                Core::Math::Matrix4::OutputScale(&scaleMat, in_fSize, in_fSize, 1.0f);

                Core::Math::Matrix4 transMat;
                Core::Math::Matrix4::OutputTranslation(
                    &transMat,
                    Core::Math::Vector3((in_rPos._fX - fOffset) - (in_rViewConfig._uWidth >> 1),
                                        -(in_rPos._fY - fOffset) + (in_rViewConfig._uHeight >> 1),
                                        0.0f));

                Core::Math::Matrix4 world;
                world.Set(scaleMat);

                world.Mul(transMat);
                pMat->SetPropertyMatrix("uWorldTransform", world);
            }

            // 白色の1x1テクスチャを設定
            pMat->SetPropertyTexture("uTexture", pWhiteTex);

            // 色設定
            {
                const Core::Math::Color32& rC32 = in_color.c32;
                Core::Math::Vector4
                    _color(static_cast<HE::Float32>(rC32.r) * Core::Math::fInvert255,
                           static_cast<HE::Float32>(rC32.g) * Core::Math::fInvert255,
                           static_cast<HE::Float32>(rC32.b) * Core::Math::fInvert255,
                           static_cast<HE::Float32>(rC32.a) * Core::Math::fInvert255);

                pMat->SetPropertyVector4("uColor", _color);
            }

            auto pMesh = reinterpret_cast<Mesh*>(this->_p2DCircleMesh);
            pMesh->DrawByVertexOnly(GL_TRIANGLE_FAN);
        }
        pWhiteTex->Disable();
        pMat->Disable();
    }

    void Screen::V2DDrawTriangle(const Platform::ViewPortConfig& in_rViewConfig,
                                 const Core::Math::Vector2& in_rPos,
                                 const Core::Math::EAnchor in_eAchor,
                                 const HE::Float32 in_fAngleDegress, const HE::Float32 in_fSize,
                                 const Core::Math::Color in_color)
    {
        auto pMat = reinterpret_cast<Material*>(this->_p2DGeometoryMat);
        pMat->Enable();

        auto pWhiteTex = reinterpret_cast<TextureSurface*>(this->_pWhiteTex);
        pWhiteTex->Enable();
        {
            // 射影はシンプルなのを利用
            {
                Core::Math::Matrix4 viewProj;
                Core::Math::Matrix4::OutputSimpleViewProj(&viewProj,
                                                          static_cast<HE::Float32>(
                                                              in_rViewConfig._uWidth),
                                                          static_cast<HE::Float32>(
                                                              in_rViewConfig._uHeight));
                pMat->SetPropertyMatrix("uViewProj", viewProj);
            }

            // 座標を設定
            {
                HE::Float32 fOffset = 0.0f;
                switch (in_eAchor)
                {
                    case Core::Math::EAnchor_Left:
                    {
                        break;
                    }

                    case Core::Math::EAnchor_Center:
                    {
                        fOffset = in_fSize * 0.5f;
                        break;
                    }
                }

                Core::Math::Matrix4 scaleMat;
                Core::Math::Matrix4::OutputScale(&scaleMat, in_fSize, in_fSize, 1.0f);

                Core::Math::Matrix4 rotZMat;
                Core::Math::Matrix4::OutputRotationZ(&rotZMat, Core::Math::DegreesToRadians(
                                                                   in_fAngleDegress));

                Core::Math::Matrix4 transMat;
                Core::Math::Matrix4::OutputTranslation(
                    &transMat,
                    Core::Math::Vector3((in_rPos._fX - fOffset) - (in_rViewConfig._uWidth >> 1),
                                        -(in_rPos._fY - fOffset) + (in_rViewConfig._uHeight >> 1),
                                        0.0f));

                Core::Math::Matrix4 world;
                world.Set(scaleMat);

                world.Mul(rotZMat);

                world.Mul(transMat);
                pMat->SetPropertyMatrix("uWorldTransform", world);
            }

            // 白色の1x1テクスチャを設定
            pMat->SetPropertyTexture("uTexture", pWhiteTex);

            // 色設定
            {
                const Core::Math::Color32& rC32 = in_color.c32;
                Core::Math::Vector4
                    _color(static_cast<HE::Float32>(rC32.r) * Core::Math::fInvert255,
                           static_cast<HE::Float32>(rC32.g) * Core::Math::fInvert255,
                           static_cast<HE::Float32>(rC32.b) * Core::Math::fInvert255,
                           static_cast<HE::Float32>(rC32.a) * Core::Math::fInvert255);

                pMat->SetPropertyVector4("uColor", _color);
            }

            auto pMesh = reinterpret_cast<Mesh*>(this->_p2DTriangleMesh);
            pMesh->DrawByVertexOnly(GL_TRIANGLES);
        }
        pWhiteTex->Disable();
        pMat->Disable();
    }

}  // namespace PlatformSDL2
