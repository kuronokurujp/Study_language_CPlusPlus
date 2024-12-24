#include "Window.h"

#include "./Scene.h"
#include "./ViewPort.h"
#include "Engine/Engine.h"
#include "Engine/Platform/PlatformModule.h"

namespace Render
{
    static void _RenderCommand(const Render::Command* in_pCommand,
                               const Platform::ViewPortConfig& in_rViewPortConfig,
                               Platform::ScreenInterface* in_pScreen)
    {
        // TODO: コマンドに応じた描画処理をする
        switch (in_pCommand->_uType)
        {
            // 画面クリア
            case Render::ECmdType_ClsScreen:
            {
                const Render::CmdClsScreen* pClsScreen = &in_pCommand->_data._clsScree;
                const auto& rColor                     = pClsScreen->_color;

                in_pScreen->VCls(rColor.c32.r, rColor.c32.b, rColor.c32.g);
                break;
            }

            // 矩形を描画
            case Render::ECmdType_2DQuadDraw:
            {
                const Render::Cmd2DQuadDraw* pRect2D = &in_pCommand->_data._2DDrawRect;

                Core::Math::Rect2 r;
                r.SetRect(pRect2D->_fLeftX, pRect2D->_fRightX, pRect2D->_fLeftY, pRect2D->_fRightY,
                          Core::Math::EAnchor::EAnchor_Left);

                in_pScreen->V2DDrawQuad(in_rViewPortConfig, r, pRect2D->_color);
                break;
            }

            // 点群描画
            case Render::ECmdType_2DPointArrayDraw:
            {
                // データ置換
                const Render::Cmd2DPointArrayDraw* pCmdPoint2DCloud =
                    &in_pCommand->_data._2DDrawPointCloud;
                HE_ASSERT(0 < pCmdPoint2DCloud->_uCount && "点群の点が一つもないのはだめ");

                if (0 < pCmdPoint2DCloud->_uCount)
                {
                    /*
                        const HE::Uint32 num =
                            HE_MIN(pCmdPoint2DCloud->uCount, s_u2DPointCount);
                            */
                    /*
                    std::transform(pCmdPoint2DCloud->aPoint,
                    pCmdPoint2DCloud->aPoint + num, s_a2DPoint,
                                   [](const Render::Point2D& src)
                                   {
                                       const auto& rColor = src.color;
                                       const auto uColor =
                                           ::GetColor(rColor.c32.r, rColor.c32.g,
                                                      rColor.c32.b);
                                       return ::POINTDATA{static_cast<int>(src.fX),
                                                          static_cast<int>(src.fY),
                    uColor, 0};

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
                const Render::Cmd2DPointDraw* pCmdPoint2D = &in_pCommand->_data._2DDrawPoint;

                const Render::Point2D* pPoint2D = &pCmdPoint2D->_point;
                const auto& rColor              = pPoint2D->_color;
                /*
                const auto uColor = ::GetColor(rColor.c32.r, rColor.c32.g,
                rColor.c32.b);
                ::DrawPixel(static_cast<int>(pPoint2D->fX),
                static_cast<int>(pPoint2D->fY), uColor);
                            */

                break;
            }

            // 2次元の円描画
            case Render::ECmdType_2DCircleDraw:
            {
                const Render::Cmd2DCircleDraw* pCmd = &in_pCommand->_data._2DDrawCircle;

                const Render::Point2D& rPoint = pCmd->_point;
                in_pScreen->V2DDrawCircle(in_rViewPortConfig,
                                          Core::Math::Vector2(rPoint._fX, rPoint._fY), pCmd->_eAnchor,
                                          pCmd->_fSize, rPoint._color);
                break;
            }

            // 2Dテキストを描画
            case Render::ECmdType_2DTextDraw:
            {
                const Render::Cmd2DTextDraw* pCmd = &in_pCommand->_data._2DDrawText;

                in_pScreen->V2DDrawText(in_rViewPortConfig, Core::Math::Vector2(pCmd->_fX, pCmd->_fY),
                                        pCmd->_eAnchor, pCmd->_szChars, pCmd->_uSize, pCmd->_color);
                break;
            }

            // TODO: 2Dの三角形を描画
            case Render::ECmdType_2DTriangleDraw:
            {
                const Render::Cmd2DTriangleDraw* pCmd = &in_pCommand->_data._2DDrawTriangle;
                const Render::Point2D& rPoint         = pCmd->_point;
                in_pScreen->V2DDrawTriangle(in_rViewPortConfig,
                                            Core::Math::Vector2(rPoint._fX, rPoint._fY),
                                            pCmd->_eAnchor, pCmd->_fAngleDegrees, pCmd->_fSize, rPoint._color);

                break;
            }
        }
    }

    Core::Common::Handle Window::AddViewPort(
        Core::Memory::UniquePtr<Platform::ViewPortStrategy> in_upStg)
    {
        auto [handle, pViewPort] = this->_poolViewPortManager.Alloc<ViewPort>();

        // TODO: 用意できるシーン数は固定にしておく
        pViewPort->Init(std::move(in_upStg), 3);

        return handle;
    }

    HE::Bool Window::RemoveViewPort(Core::Common::Handle& in_rHandle)
    {
        auto pViewPort = this->_poolViewPortManager.Ref(in_rHandle);
        HE_ASSERT(pViewPort);

        pViewPort->_End();
        pViewPort->Release();

        this->_poolViewPortManager.Free(in_rHandle, FALSE);

        return TRUE;
    }

    void Window::Show()
    {
        if (this->_bReady) this->_upStrategy->VShow();
        this->_bShow = TRUE;
    }

    HE::Bool Window::Init(Core::Memory::UniquePtr<Platform::WindowStrategy> in_upConfig)
    {
        this->_upStrategy = std::move(in_upConfig);

        auto rWindowConfig = this->_upStrategy->GetConfig();
        HE_ASSERT(0 < rWindowConfig.uViewPortCount);

        this->_poolViewPortManager.ReleasePool();
        this->_poolViewPortManager.ReservePool(rWindowConfig.uViewPortCount);

        return TRUE;
    }

    void Window::Release()
    {
        this->_upStrategy->VRelease();
        this->_poolViewPortManager.ReleasePool([](ViewPort* in_pViewPort)
                                               { in_pViewPort->Release(); });

        HE_SAFE_DELETE_UNIQUE_PTR(this->_upStrategy);
    }

    void Window::_Begin()
    {
        this->_upStrategy->VBegin();
        if (this->_bShow)
        {
            this->_upStrategy->VShow();
        }
        this->_bReady = TRUE;
    }

    void Window::_End()
    {
        this->_upStrategy->VEnd();

        {
            auto m = this->_poolViewPortManager.GetUserDataList();
            HE_ASSERT_RETURN(m);

            for (auto itr = m->begin(); itr != m->end(); ++itr)
            {
                itr->second->_End();
            }
        }
    }

    void Window::_Update(const HE::Float32 in_fDt)
    {
        // ビューポート処理
        auto m = this->_poolViewPortManager.GetUserDataList();
        if (m)
        {
            for (auto itrViewPort = m->begin(); itrViewPort != m->end(); ++itrViewPort)
            {
                auto pViewPort = itrViewPort->second;

                // シーン更新
                // シーンの実際の更新はストラテジークラス側で行う
                auto m = pViewPort->_poolSceneManager.GetUserDataList();
                if (m == NULL) continue;

                for (auto itrScene = m->begin(); itrScene != m->end(); ++itrScene)
                {
                    itrScene->second->_Update(in_fDt);
                }
            }
        }
    }

    void Window::_Render()
    {
        auto pPlatformModule = HE_ENGINE.PlatformModule();
        HE_ASSERT(pPlatformModule);

        auto pPlatformScreen = pPlatformModule->VScreen();

        this->_upStrategy->VBeginRender();

        // ビューポート処理
        auto m = this->_poolViewPortManager.GetUserDataList();
        if (m)
        {
            for (auto itrViewPort = m->begin(); itrViewPort != m->end(); ++itrViewPort)
            {
                // シーン描画
                auto pViewPort = itrViewPort->second;
                auto m         = pViewPort->_poolSceneManager.GetUserDataList();
                if (m == NULL) continue;

                pViewPort->_BeginRender();

                Platform::ViewPortConfig viewPortConfig;
                {
                    viewPortConfig._uWidth  = pViewPort->Width();
                    viewPortConfig._uHeight = pViewPort->Height();
                }

                for (auto itrScene = m->begin(); itrScene != m->end(); ++itrScene)
                {
                    itrScene->second->_BeginRender();

                    // ビュー毎に描画コマンド処理
                    const Render::Command* pCommand = itrScene->second->_commandBuff.PopBack();
                    while (pCommand != NULL)
                    {
                        _RenderCommand(pCommand, viewPortConfig, pPlatformScreen.get());

                        pCommand = itrScene->second->_commandBuff.PopBack();
                    }

                    itrScene->second->_EndRender();
                }

                pViewPort->_EndRender();
            }
        }
        this->_upStrategy->VEndRender();
    }

}  // namespace Render
