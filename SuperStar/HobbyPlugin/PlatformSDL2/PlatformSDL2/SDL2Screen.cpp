#pragma once

#include "SDL2Screen.h"

#include <algorithm>

#include "SDL2/SDL.h"

// 依存モジュール一覧
#include "PlatformSDL2Module.h"
#include "RenderModule.h"

namespace PlatformSDL2
{
    /// <summary>
    /// DXLibのウィンドウロジック
    /// </summary>
    class DXLibWindowStrategy final : public Render::WindowStrategy
    {
    public:
        DXLibWindowStrategy()
        {
            // TODO: 設定は仮
            this->_uViewPortCount = 1;
            this->_uHeight        = 480;
            this->_uWidht         = 640;
        }

    protected:
        void _VBegin() override final
        {
            // windowを作成
            // openglを扱うようにする
            this->_pWindow =
                SDL_CreateWindow("", 0, 0, this->_uWidht, this->_uHeight, SDL_WINDOW_OPENGL);

            // Windowに紐づいているOpenGLのコンテキストを生成
            // これを利用してOpenGLの機能を使う
            this->_glContext = SDL_GL_CreateContext(this->_pWindow);
            HE_ASSERT(this->_glContext);
        }

        void _VEnd() override final
        {
            SDL_GL_DeleteContext(this->_glContext);

            if (this->_pWindow)
            {
                SDL_DestroyWindow(this->_pWindow);
                this->_pWindow = NULL;
            }
        }

        void _VShow() override final {}

        void _VBeginRender() override final
        {
            /*
                // カラーバッファをクリアする
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // 3D描画開始
                glEnable(GL_DEPTH_TEST);
                glDisable(GL_BLEND);
                */
        }

        void _VEndRender() override final
        {
            // ウィンドウの描画バッファを切り替える
            SDL_GL_SwapWindow(this->_pWindow);
        }

    private:
        SDL_Window* _pWindow = NULL;
        SDL_GLContext _glContext;
    };

    // TODO: 2D用のシーン
    class DXLibSceneView2D : public Render::SceneViewBase
    {
    protected:
        HE::Bool _VBegin() override final { return TRUE; }
        void _VEnd() override final {}

        void _VUpdate(const HE::Float32) override final {}

        void _VRender(Render::ViewPort* in_pViewPort) override final
        {
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
                            const Uint32 num = HE_MIN(pCmdPoint2DCloud->uCount, s_u2DPointCount);
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

    private:
        // 2Dの点群を表示するために必要
        static constexpr Uint32 s_u2DPointCount = 500;
        // static inline ::POINTDATA s_a2DPoint[s_u2DPointCount];
    };

    // TODO: UI用のシーン
    class DXLibSceneViewUI final : public DXLibSceneView2D
    {
    };

    Screen::Screen(PlatformSDL2::PlatformSDL2Module* in_pSDL2Module)
    {
        HE_ASSERT(in_pSDL2Module);

        this->_pSDL2Module = in_pSDL2Module;
    }

    const Core::Common::Handle Screen::VCreateWindow()
    {
        auto pRenderModule = this->_pSDL2Module->GetDependenceModule<Render::RenderModule>();
        if (pRenderModule == NULL) return FALSE;

        auto upWindowStrategy = HE_MAKE_CUSTOM_UNIQUE_PTR((DXLibWindowStrategy));
        auto handle           = pRenderModule->NewWindow(std::move(upWindowStrategy));

        this->_vWindowHandle.PushBack(handle);

        return handle;
    }

    HE::Bool Screen::VReleaseAllWindows()
    {
        auto pRenderModule = this->_pSDL2Module->GetDependenceModule<Render::RenderModule>();
        HE_ASSERT_RETURN_VALUE(FALSE, pRenderModule);

        pRenderModule->DeleteAllWindow();

        return TRUE;
    }

    Core::Common::Handle Screen::VAddViewPort(const Core::Common::Handle& in_rWindowHandle)
    {
        auto pRenderModule = this->_pSDL2Module->GetDependenceModule<Render::RenderModule>();
        HE_ASSERT_RETURN_VALUE(NullHandle, pRenderModule);

        Core::Memory::UniquePtr<Render::ViewPortConfig> upConfig =
            HE_MAKE_CUSTOM_UNIQUE_PTR((Render::ViewPortConfig));
        // TODO: 用意できるシーン数(仮)
        upConfig->uSceneCount = 4;

        return pRenderModule->AddViewPort(in_rWindowHandle, std::move(upConfig));
    }

    void Screen::VRemoveViewPort(const Core::Common::Handle& in_rWindowHandle,
                                 const Core::Common::Handle& in_rViewPortHandle)
    {
        auto pRenderModule = this->_pSDL2Module->GetDependenceModule<Render::RenderModule>();
        HE_ASSERT_RETURN(pRenderModule);

        pRenderModule->RemoveViewPort(in_rWindowHandle, in_rViewPortHandle);
    }

    const Core::Common::Handle& Screen::VAddSceneViewUI(
        const Platform::ScreenSceneViewUIConfig& in_rConfig)
    {
        // TODO: プラットフォーム専用のUIシーンを追加
        auto pRenderModule = this->_pSDL2Module->GetDependenceModule<Render::RenderModule>();
        HE_ASSERT_RETURN_VALUE(NullHandle, pRenderModule);

        const Core::Common::Handle& handle =
            pRenderModule->AddSceneViewUI<DXLibSceneViewUI>(in_rConfig._windowHandle,
                                                            in_rConfig._viewPortHandle);
        return handle;
    }

    const Core::Common::Handle& Screen::VAddSceneView2D(
        const Platform::ScreenSceneView2DConfig& in_rConfig)
    {
        // TODO: プラットフォーム専用の2Dシーンを追加
        auto pRenderModule = this->_pSDL2Module->GetDependenceModule<Render::RenderModule>();
        HE_ASSERT_RETURN_VALUE(NullHandle, pRenderModule);

        return pRenderModule->AddSceneView2D<DXLibSceneView2D>(in_rConfig._windowHandle,
                                                               in_rConfig._viewPortHandle);
    }

    const Platform::ScreenSceneView2DEnvironment&& Screen::GetEnvBySceneView2D(
        const Core::Common::Handle& in_rHandle)
    {
        auto env = Platform::ScreenSceneView2DEnvironment();

        // TODO: プラットフォーム専用の2Dシーンを追加
        auto pRenderModule = this->_pSDL2Module->GetDependenceModule<Render::RenderModule>();
        HE_ASSERT_RETURN_VALUE(std::move(env), pRenderModule);

        // TODO: ビューポートの設定情報取得
        // 設定情報から環境情報を構築
        auto pViewPortConfig = pRenderModule->GetViewPortConfig(in_rHandle);

        env._uWidth  = pViewPortConfig->uWidth;
        env._uHeight = pViewPortConfig->uHeight;

        return std::move(env);
    }

    void Screen ::VShowWindow(const Core::Common::Handle& in_rHandle)
    {
        auto pRenderModule = this->_pSDL2Module->GetDependenceModule<Render::RenderModule>();
        HE_ASSERT_RETURN(pRenderModule);

        pRenderModule->ShowWindow(in_rHandle);
    }

}  // namespace PlatformSDL2
