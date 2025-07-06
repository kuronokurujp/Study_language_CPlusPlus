#include "Scene.h"

#include "./Render/Particle/Blob.h"
#include "Engine/Platform/PlatformModule.h"

namespace Render
{
    SceneViewBase::SceneViewBase()
    {
        this->_runtimePoolPrticleBlob.POOL_RESERVE_POOL(1024);
    }

    HE::Bool SceneViewBase::Init(Core::Memory::UniquePtr<Platform::SceneStrategyInterface> in_upSt)
    {
        this->_upSt = std::move(in_upSt);
        return TRUE;
    }

    void SceneViewBase::Release()
    {
        this->_runtimePoolPrticleBlob.ReleasePool();
        HE_SAFE_DELETE_UNIQUE_PTR(this->_upSt);
    }

    const Render::Particle::Blob& SceneViewBase::GetPrticle(const Core::Common::Handle in_handle)
    {
        return *(this->_runtimePoolPrticleBlob.Ref(in_handle));
    }

    void SceneViewBase::_Begin()
    {
        if (this->_upSt->VBegin() == FALSE)
        {
        }
    }

    void SceneViewBase::_End()
    {
        this->_upSt->VEnd();
    }

    void SceneViewBase::_Update(const HE::Float32 in_fDt)
    {
        this->_upSt->VUpdate(in_fDt);
    }

    void SceneViewBase::_BeginRender()
    {
        this->_upSt->VBeginRender();
    }

    void SceneViewBase::_EndRender()
    {
        this->_upSt->VEndRender();
    }

    void SceneViewBase::_Render(const Platform::ViewPortConfig& in_rViewPortConfig)
    {
        // 戦略からプラットフォームを取得してレンダリング
        auto pPlatformScreen = this->_upSt->VGetRenderer();
        if (pPlatformScreen == NULL) return;

        // ビュー毎に描画コマンド処理
        const Render::Command* pCommand = this->_commandBuff.PopBack();
        while (pCommand != NULL)
        {
            // コマンドに応じた描画処理をする
            switch (pCommand->_uType)
            {
                // 画面クリア
                case Render::ECmdType_ClsScreen:
                {
                    const Render::CmdClsScreen* pClsScreen = &pCommand->_data._clsScree;
                    const auto& rColor                     = pClsScreen->_color;

                    pPlatformScreen->VCls(rColor.c32.r, rColor.c32.b, rColor.c32.g);
                    break;
                }

                // 矩形を描画
                case Render::ECmdType_2DQuadDraw:
                {
                    const Render::Cmd2DQuadDraw* pRect2D = &pCommand->_data._2DDrawRect;

                    Core::Math::Rect2 r;
                    r.SetRect(pRect2D->_fLeftX, pRect2D->_fRightX, pRect2D->_fLeftY,
                              pRect2D->_fRightY, Core::Math::EAnchor::EAnchor_Left);

                    pPlatformScreen->V2DDrawQuad(in_rViewPortConfig, r, pRect2D->_color);
                    break;
                }

                // 2次元の円描画
                case Render::ECmdType_2DCircleDraw:
                {
                    const Render::Cmd2DCircleDraw* pCmd = &pCommand->_data._2DDrawCircle;

                    pPlatformScreen->V2DDrawCircle(in_rViewPortConfig,
                                                   Core::Math::Vector2(pCmd->_fX, pCmd->_fY),
                                                   pCmd->_eAnchor, pCmd->_fSize, pCmd->_color);
                    break;
                }

                // 2Dテキストを描画
                case Render::ECmdType_2DTextDraw:
                {
                    const Render::Cmd2DTextDraw* pCmd = &pCommand->_data._2DDrawText;

                    pPlatformScreen->V2DDrawText(in_rViewPortConfig,
                                                 Core::Math::Vector2(pCmd->_fX, pCmd->_fY),
                                                 pCmd->_eAnchor, pCmd->_szChars, pCmd->_uSize,
                                                 pCmd->_uSpace, pCmd->_color);
                    break;
                }

                // 2Dの三角形を描画
                case Render::ECmdType_2DTriangleDraw:
                {
                    const Render::Cmd2DTriangleDraw* pCmd = &pCommand->_data._2DDrawTriangle;
                    pPlatformScreen->V2DDrawTriangle(in_rViewPortConfig,
                                                     Core::Math::Vector2(pCmd->_fX, pCmd->_fY),
                                                     pCmd->_eAnchor, pCmd->_fAngleDegrees,
                                                     pCmd->_fSize, pCmd->_color);

                    break;
                }

                // 2Dパーティクル描画
                case Render::ECmdType_2DParticalDraw:
                {
                    const auto pCmd = &pCommand->_data._Particle;
                    auto& p         = this->GetPrticle(pCmd->handle);
                    Core::Math::Vector3 pos(pCmd->_fX, pCmd->_fY, pCmd->_fZ);
                    pPlatformScreen->V2DDrawPartical(in_rViewPortConfig, p.GetDrawHandle(), pos);

                    break;
                }
                    // TODO: コマンドがない場合のメソッドを呼び出す
                default:
                    break;
            }

            pCommand = this->_commandBuff.PopBack();
        }
    }

    HE::Bool SceneViewBase::_PushCommand(Command&& in_rrCmd)
    {
        // コマンドをコピー
        // 要素を先頭に追加
        // 最後に追加したコマンドは配列先頭に
        // 最初に追加したコマンドは配列末尾に
        // TODO: コマンドをまとめる
        // テキスト描画コマンドなど描画コマンドはまとめるようにする
        // 描画処理をまとめる事で高速処理をする
        switch (in_rrCmd._uType)
        {
            // 画面を指定色でクリア
            case ECmdType_ClsScreen:
                break;
            // テキスト描画
            case ECmdType_2DTextDraw:
                break;
            // 矩形描画
            case ECmdType_2DQuadDraw:
                break;
            // 円の描画
            case ECmdType_2DCircleDraw:
                break;
            // 三角形の描画
            case ECmdType_2DTriangleDraw:
                break;
        }
        this->_commandBuff.PushFront(in_rrCmd);

        return TRUE;
    }
}  // namespace Render
