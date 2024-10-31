#include "DXLibModule.h"

#include <algorithm>

#include "DxLib.h"
#include "Engine/Memory/Memory.h"

// 依存モジュール一覧
#include "RenderModule.h"

namespace DXLib
{
    // 2Dの点群を表示するために必要
    static ::POINTDATA* s_a2DPoint      = NULL;
    static const Uint32 s_u2DPointCount = 500;

    DXLibModule::DXLibModule() : PlatformModule()
    {
        this->_AppendDependenceModule<Render::RenderModule>();
    }

    Bool DXLibModule::_VStart()
    {
        Bool bRet = TRUE;
        {
            // スクリーン初期化はDxLibの初期化前に必要
            if (this->_screen.Init() == FALSE) return FALSE;

            // 初期化と同時にウィンドウが表示するので非表示にする
            // 表示はウィンドウ生成メソッドが呼ばれたら
            // ウィンドウを非表示にするとアプリが非アクティブになってしまうので
            // 一時的に非アクティブでも動作するようにします
            ::SetAlwaysRunFlag(TRUE);

            // ウィンドウを非表示にします
            ::SetWindowVisibleFlag(FALSE);

            // DxLib初期化
            if (::DxLib_Init() == -1)
            {
                bRet = FALSE;
            }
            else
            {
                // デバッグ時のみログファイルを出す
#ifdef _DEBUG
                ::SetOutApplicationLogValidFlag(TRUE);
#else
                SetOutApplicationLogValidFlag(FALSE);
#endif
            }

            this->_input.VInit();
        }

        HE_ASSERT(s_a2DPoint == NULL && "モジュールが重複生成されている");
        s_a2DPoint = HE_NEW_MEM_ARRAY(::POINTDATA, s_u2DPointCount, 0);

        return bRet;
    }

    Bool DXLibModule::_VRelease()
    {
        HE_SAFE_DELETE_MEM_ARRAY(s_a2DPoint);

        // DxLibの後始末
        ::DxLib_End();

        return TRUE;
    }

    Bool DXLibModule::VCreateMainWindow()
    {
        // ウィンドウを表示する
        ::SetWindowVisibleFlag(TRUE);

        // 非アクティブでも実行する設定を解除します
        ::SetAlwaysRunFlag(FALSE);

        return TRUE;
    }

    Bool DXLibModule::VReleaseAllWindows()
    {
        return FALSE;
    }

    Bool DXLibModule::_VBeforeUpdate(const Float32 in_fDeltaTime)
    {
        {
            // 入力更新
            this->_input.VUpdate(in_fDeltaTime);

            // 0以外だと画面終了メッセージとみなしている
            if ((ProcessMessage() != 0))
            {
                this->_bQuit = TRUE;
            }
        }

        return (this->_bQuit == FALSE);
    }

    Bool DXLibModule::_VUpdate(const Float32 in_fDeltaTime)
    {
        if (this->_bQuit) return FALSE;

        return TRUE;
    }

    Bool DXLibModule::_VLateUpdate(const Float32 in_fDeltaTime)
    {
        if (this->_bQuit) return FALSE;

        // 画面全体の色を初期化
        ::SetBackgroundColor(0, 0, 0);

        // 画面の描画クリア
        ::ClearDrawScreen();

        auto pRenderModule = this->GetDependenceModule<Render::RenderModule>();
        HE_ASSERT(pRenderModule);

        // ビュー毎に描画コマンド処理
        auto vViewHandles = pRenderModule->ViewHandles();
        for (Uint32 i = 0; i < vViewHandles.Size(); ++i)
        {
            auto pView = pRenderModule->GetView(vViewHandles[i]);

            const Render::Command* pCommand = pView->PopCmd();
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

                        // 画面全体の色を初期化
                        ::SetBackgroundColor(rColor.c32.r, rColor.c32.g, rColor.c32.b);

                        // 画面の描画クリア
                        ::ClearDrawScreen();

                        break;
                    }

                    // 矩形を描画
                    case Render::ECmdType_2DRectDraw:
                    {
                        const Render::Cmd2DRectDraw* pRect2D = &pCommand->data.rect2DDraw;

                        const Uint32 uColor = ::GetColor(pRect2D->color.c32.r, pRect2D->color.c32.g,
                                                         pRect2D->color.c32.b);
                        // TODO: 整数版と切り替えた方がいいのだろうか
                        ::DrawBoxAA(pRect2D->fLeftX, pRect2D->fLeftY, pRect2D->fRightX,
                                    pRect2D->fRightY, uColor, TRUE);
                        break;
                    }

                    // 2Dテキストを描画
                    case Render::ECmdType_2DTextDraw:
                    {
                        const Render::Cmd2DTextDraw* pText2D = &pCommand->data.text2DDraw;

                        const Uint32 uColor = ::GetColor(pText2D->color.c32.r, pText2D->color.c32.g,
                                                         pText2D->color.c32.b);
                        Core::Common::FixString1024 str(pText2D->szChars);
                        int x = static_cast<int>(pText2D->fX);
                        int y = static_cast<int>(pText2D->fY);
                        // 配置指定で配置座標を変更
                        switch (pText2D->anchor)
                        {
                            // 中央位置
                            case Core::Math::Rect2::EAnchor_Center:
                            {
                                const int sWidth =
                                    ::GetDrawStringWidth(pText2D->szChars,
                                                         HE_STR_LEN(pText2D->szChars));
                                const int sHeight = ::GetDrawStringWidth(pText2D->szChars, 1);
                                if ((sWidth != -1) && (sHeight != -1))
                                {
                                    x -= (sWidth >> 1);
                                    y -= (sHeight >> 1);
                                }
                                break;
                            }
                            // 左上位置
                            case Core::Math::Rect2::EAnchor_Left:
                            {
                                break;
                            }
                        }

                        ::DrawString(x, y, pText2D->szChars, uColor);
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
                            const Uint32 num = HE_MIN(pCmdPoint2DCloud->uCount, s_u2DPointCount);
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
                        }
                        break;
                    }

                    // 点描画
                    case Render::ECmdType_2DPointDraw:
                    {
                        const Render::Cmd2DPointDraw* pCmdPoint2D = &pCommand->data.point2DDraw;

                        const Render::Point2D* pPoint2D = &pCmdPoint2D->point;
                        const auto& rColor              = pPoint2D->color;
                        const auto uColor = ::GetColor(rColor.c32.r, rColor.c32.g, rColor.c32.b);
                        ::DrawPixel(static_cast<int>(pPoint2D->fX), static_cast<int>(pPoint2D->fY),
                                    uColor);

                        break;
                    }

                    // 2次元の円描画
                    case Render::ECmdType_2DCircleDraw:
                    {
                        const Render::Cmd2DCircleDraw* pCmdCircle = &pCommand->data.circle2DDraw;
                        const Render::Color* pColor               = &pCmdCircle->color;
                        const Uint32 uColor =
                            ::GetColor(pColor->c32.r, pColor->c32.g, pColor->c32.b);

                        // 円を描画
                        ::DrawCircleAA(pCmdCircle->point.fX, pCmdCircle->point.fY,
                                       pCmdCircle->fSize, 32, uColor, TRUE);

                        break;
                    }

                    default:
                        HE_ASSERT(0 && "存在しないコマンド");
                }

                pCommand = pView->PopCmd();
            }
        }

        // 画面の反映
        ScreenFlip();

        return TRUE;
    }

}  // namespace DXLib
