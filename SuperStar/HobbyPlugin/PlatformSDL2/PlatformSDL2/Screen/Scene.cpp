#include "Scene.h"

#include "./Draw/Material.h"
#include "./Draw/Mesh.h"
#include "Engine/Math/Math.h"
#include "GL/glew.h"
#include "SDL2/SDL.h"

namespace PlatformSDL2
{
#if 0
    // 頂点データを格納する構造体
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
    static std::vector<GLuint> s_indices;

#if 0
    void SceneView2D::VRender()
    {
        /*
                // ビュー毎に描画コマンド処理
                const Render::Command* pCommand = this->_commandBuff.PopBack();
                while (pCommand != NULL)
                {
                    this->_VRenderFromCommand(pCommand, in_pViewPort);

                    pCommand = this->_commandBuff.PopBack();

                    s_vertices.clear();
                    s_indices.clear();
                }
                */
    }
#endif

#if 0
    void SceneView2D::_VRenderFromCommand(const Render::Command* in_pCommand,
                                          const Render::ViewPort* in_pViewPort)
    {
        // コマンドに応じた描画処理をする
        switch (in_pCommand->uType)
        {
            // 画面クリア
            case Render::ECmdType_ClsScreen:
            {
                const Render::CmdClsScreen* pClsScreen = &in_pCommand->data.clsScree;
                const auto& rColor                     = pClsScreen->color;

                // 色は0 - 1.0
                glClearColor(rColor.c32.RedRate(), rColor.c32.GreenRate(), rColor.c32.BlueRate(),
                             rColor.c32.AlphaRate());

                break;
            }

            // 矩形を描画
            case Render::ECmdType_2DRectDraw:
            {
                const Render::Cmd2DRectDraw* pRect2D = &in_pCommand->data.rect2DDraw;

                break;
            }

            // 点群描画
            case Render::ECmdType_2DPointArrayDraw:
            {
                // データ置換
                const Render::Cmd2DPointArrayDraw* pCmdPoint2DCloud =
                    &in_pCommand->data.pointCloud2DDraw;
                HE_ASSERT(0 < pCmdPoint2DCloud->uCount && "点群の点が一つもないのはだめ");

                if (0 < pCmdPoint2DCloud->uCount)
                {
                    const HE::Uint32 num = HE_MIN(pCmdPoint2DCloud->uCount, s_u2DPointCount);
                    /*
                    std::transform(pCmdPoint2DCloud->aPoint, pCmdPoint2DCloud->aPoint + num,
                                   s_a2DPoint,
                                   [](const Render::Point2D& src)
                                   {
                                       const auto& rColor = src.color;
                                       const auto uColor =
                                           ::GetColor(rColor.c32.r, rColor.c32.g,
                                                      rColor.c32.b);
                                       return ::POINTDATA{static_cast<int>(src.fX),
                                                          static_cast<int>(src.fY), uColor,
                                                          0};

                                   });

                    // 点の集合を描画する
                    ::DrawPixelSet(s_a2DPoint, num);
                                   */
                }
                break;
            }

            // 点描画
            case Render::ECmdType_2DPointDraw:
            {
                const Render::Cmd2DPointDraw* pCmdPoint2D = &in_pCommand->data.point2DDraw;

                const Render::Point2D* pPoint2D = &pCmdPoint2D->point;
                const auto& rColor              = pPoint2D->color;
                /*
                const auto uColor = ::GetColor(rColor.c32.r, rColor.c32.g, rColor.c32.b);
                ::DrawPixel(static_cast<int>(pPoint2D->fX), static_cast<int>(pPoint2D->fY),
                            uColor);
                            */

                break;
            }

            // 2次元の円描画
            case Render::ECmdType_2DCircleDraw:
            {
                const Render::Cmd2DCircleDraw* pCmdCircle = &in_pCommand->data.circle2DDraw;
                const Render::Color* pColor               = &pCmdCircle->color;
                /*
                const Uint32 uColor =
                    ::GetColor(pColor->c32.r, pColor->c32.g, pColor->c32.b);

                // 円を描画
                ::DrawCircleAA(pCmdCircle->point.fX, pCmdCircle->point.fY,
                               pCmdCircle->fSize, 32, uColor, TRUE);

*/
                break;
            }
        }
    }

    void SceneViewUI::_VRenderFromCommand(const Render::Command* in_pCommand,
                                          const Render::ViewPort* in_pViewPort)
    {
        SceneView2D::_VRenderFromCommand(in_pCommand, in_pViewPort);

        auto pViewPortConfig = in_pViewPort->GetConfig();
        s_vertices.clear();
        s_indices.clear();

        // コマンドに応じた描画処理をする
        switch (in_pCommand->uType)
        {
            // 2Dテキストを描画
            case Render::ECmdType_2DTextDraw:
            {
                // 色は0 - 1.0
                //::glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

                // TODO: フォントのマテリアルを取得
                auto pFontMat = reinterpret_cast<FontMaterial*>(
                    this->_spPoolMaterial->Ref(this->_fontMaterialHandle));
                HE_ASSERT(pFontMat);
                if (pFontMat == NULL) break;

                // TODO: フォントのテクスチャを取得
                auto pFontTex = this->_spPoolTexture->Ref(this->_fontTexHandle);
                HE_ASSERT(pFontTex);
                if (pFontTex == NULL) break;

                const Render::Cmd2DTextDraw* pText2D = &in_pCommand->data.text2DDraw;

                // TODO: 文字列に合わせて頂点配列を作り描画
                Mesh2D mesh;
                {
                    HE::Float32 x = 0.0f;
                    HE::Float32 y = 0.0f;

                    // インデックスオフセット
                    GLuint indexOffset = 0;
                    // TODO: 適当、後で整理
                    HE::Float32 charWidth  = 1.0f;
                    HE::Float32 charHeight = 1.0f;

                    Core::Common::g_szTempFixedString1024 = pText2D->szChars;
                    auto itrEnd = Core::Common::g_szTempFixedString1024.End();
                    for (auto itr = Core::Common::g_szTempFixedString1024.Begin(); itr != itrEnd;
                         ++itr)
                    {
                        if ((*itr)[0] == HE_STR_TEXT('\n'))
                        {
                            // 改行したら左端に戻す
                            x = 0;

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
                                uUnicode =
                                    ((c & 0x0F) << 12) | ((utf8[1] & 0x3F) << 6) | (utf8[2] & 0x3F);
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
                        s_vertices.push_back(
                            {Core::Math::Vector3(x, y, 0.0f),
                             Core::Math::Vector2(pGlpyh->_fTexSU, pGlpyh->_fTexEV)});
                        // 右下
                        s_vertices.push_back(
                            {Core::Math::Vector3(x + charWidth, y, 0.0f),
                             Core::Math::Vector2(pGlpyh->_fTexEU, pGlpyh->_fTexEV)});
                        // 右上
                        s_vertices.push_back(
                            {Core::Math::Vector3(x + charWidth, y + charHeight, 0.0f),
                             Core::Math::Vector2(pGlpyh->_fTexEU, pGlpyh->_fTexSV)});
                        // 左上
                        s_vertices.push_back(
                            {Core::Math::Vector3(x, y + charHeight, 0.0f),
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

                    // 頂点など描画に必要な情報をバインド
                    mesh.VBindFromVerticesAndIndces(s_vertices.data(), sizeof(Vertex),
                                                    s_vertices.size(), s_indices.data(),
                                                    sizeof(GLuint), s_indices.size());
                }
                // TODO: 機能有効
                {
                    // TODO: テクスチャを有効
                    // pFontTex->VEnable();
                    // TODO: マテリアルを有効
                    pFontMat->VEnable();
                }
                /*

                // TODO: 画面の縦横を設定
                // プロパティ名固定は大丈夫なのか?
                {
                    Core::Math::Matrix4 viewProj;
                    Core::Math::Matrix4::OutputSimpleViewProj(&viewProj,
                                                              static_cast<HE::Float32>(
                                                                  pViewPortConfig->uWidth),
                                                              static_cast<HE::Float32>(
                                                                  pViewPortConfig->uHeight));
                    pFontMat->VSetPropertyMatrix("uViewProj", viewProj);
                }

                // TODO: 座標を設定
                {
                    Core::Math::Matrix4 scaleMat;
                    Core::Math::Matrix4::OutputScale(&scaleMat, 32.0f, 32.0f, 1.0f);

                    Core::Math::Matrix4 transMat;
                    Core::Math::Matrix4::OutputTranslation(&transMat,
                                                           Core::Math::Vector3(pText2D->fX,
                                                                               pText2D->fY, 0.0f));

                    Core::Math::Matrix4 world;
                    world.Set(scaleMat);

                    world.Mul(transMat);
                    // プロパティ名固定は大丈夫なのか?
                    pFontMat->VSetPropertyMatrix("uWorldTransform", world);
                }
                */

                // TODO: バインドやプロパティ設定などすべての準備が整ったので描画
                mesh.VDraw();

                // TODO: 機能無効
                {
                    // TODO: マテリアルを無効
                    // pFontMat->VDisable();
                    // TODO: テクスチャを無効
                    // pFontTex->VDisable();
                }

                break;
            }
        }
    }
#endif
#endif
    HE::Bool SDL2SceneStrategy2D::VBegin()
    {
        return TRUE;
    }

    void SDL2SceneStrategy2D::VEnd()
    {
    }

    void SDL2SceneStrategy2D::VUpdate(const HE::Float32)
    {
    }

    void SDL2SceneStrategy2D::VBeginRender()
    {
        // 描画のブレンド設定
        ::glDisable(GL_DEPTH_TEST);
        ::glEnable(GL_BLEND);

        /*
                ::glDisable(GL_CULL_FACE);
                HE_ASSERT(glGetError() == GL_NO_ERROR);
                */

        // アルファブレンド設定
        // srcFactor * srcAlpha + (1 - srcAlpha) * dstFactor
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void SDL2SceneStrategy2D::VEndRender()
    {
    }
}  // namespace PlatformSDL2
