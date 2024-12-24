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
        switch (in_pCommand->uType)
        {
            // 画面クリア
            case Render::ECmdType_ClsScreen:
            {
                const Render::CmdClsScreen* pClsScreen = &in_pCommand->data.clsScree;
                const auto& rColor                     = pClsScreen->color;

                in_pScreen->VCls(rColor.c32.r, rColor.c32.b, rColor.c32.g);
                break;
            }

            // 矩形を描画
            case Render::ECmdType_2DQuadDraw:
            {
                const Render::Cmd2DQuadDraw* pRect2D = &in_pCommand->data.rect2DDraw;

                Core::Math::Rect2 r;
                r.SetRect(pRect2D->fLeftX, pRect2D->fRightX, pRect2D->fLeftY, pRect2D->fRightY,
                          Core::Math::EAnchor::EAnchor_Left);

                in_pScreen->VDrawQuad2D(in_rViewPortConfig, r, pRect2D->color);
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
                const Render::Cmd2DPointDraw* pCmdPoint2D = &in_pCommand->data.point2DDraw;

                const Render::Point2D* pPoint2D = &pCmdPoint2D->point;
                const auto& rColor              = pPoint2D->color;
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
                const Render::Cmd2DCircleDraw* pCmdCircle = &in_pCommand->data.circle2DDraw;

                const Render::Point2D& rPoint = pCmdCircle->point;
                in_pScreen->VDrawCircle2D(in_rViewPortConfig,
                                          Core::Math::Vector2(rPoint.fX, rPoint.fY),
                                          pCmdCircle->eAnchor, pCmdCircle->fSize, rPoint.color);
                break;
            }
            // 2Dテキストを描画
            case Render::ECmdType_2DTextDraw:
            {
                const Render::Cmd2DTextDraw* pText2D = &in_pCommand->data.text2DDraw;

                in_pScreen->VDrawText2D(in_rViewPortConfig,
                                        Core::Math::Vector2(pText2D->fX, pText2D->fY),
                                        pText2D->anchor, pText2D->szChars, pText2D->uSize,
                                        pText2D->color);
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
