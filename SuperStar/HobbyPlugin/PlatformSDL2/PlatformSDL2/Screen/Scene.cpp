#include "Scene.h"

#include "GL/glew.h"
#include "SDL2/SDL.h"

namespace PlatformSDL2
{
    HE::Bool SceneView2D::_VBegin()
    {
        return TRUE;
    }

    void SceneView2D::_VEnd()
    {
    }

    void SceneView2D::_VUpdate(const HE::Float32)
    {
    }

    void SceneView2D::_VRender(Render::ViewPort* in_pViewPort)
    {
        // 描画のブレンド設定
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);

        // アルファブレンド設定
        // srcFactor * srcAlpha + (1 - srcAlpha) * dstFactor
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
        // ビュー毎に描画コマンド処理
        const Render::Command* pCommand = this->_commandBuff.PopBack();
        while (pCommand != NULL)
        {
            // コマンドに応じた描画処理をする
            switch (pCommand->uType)
            {
                // 画面クリア
                case Render::ECmdType_ClsScreen:
                {
                    const Render::CmdClsScreen* pClsScreen = &pCommand->data.clsScree;
                    const auto& rColor                     = pClsScreen->color;

                    // 色は0 - 1.0
                    glClearColor(rColor.c32.RedRate(), rColor.c32.GreenRate(),
                                 rColor.c32.BlueRate(), rColor.c32.AlphaRate());

                    break;
                }

                // 矩形を描画
                case Render::ECmdType_2DRectDraw:
                {
                    const Render::Cmd2DRectDraw* pRect2D = &pCommand->data.rect2DDraw;

                    break;
                }

                // 2Dテキストを描画
                case Render::ECmdType_2DTextDraw:
                {
                    const Render::Cmd2DTextDraw* pText2D = &pCommand->data.text2DDraw;

                    break;
                }

                // 点群描画
                case Render::ECmdType_2DPointArrayDraw:
                {
                    // データ置換
                    const Render::Cmd2DPointArrayDraw* pCmdPoint2DCloud =
                        &pCommand->data.pointCloud2DDraw;
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
                    const Render::Cmd2DPointDraw* pCmdPoint2D = &pCommand->data.point2DDraw;

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
                    const Render::Cmd2DCircleDraw* pCmdCircle = &pCommand->data.circle2DDraw;
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

                default:
                    HE_ASSERT(0 && "存在しないコマンド");
            }

            pCommand = this->_commandBuff.PopBack();
        }
    }
}  // namespace PlatformSDL2
