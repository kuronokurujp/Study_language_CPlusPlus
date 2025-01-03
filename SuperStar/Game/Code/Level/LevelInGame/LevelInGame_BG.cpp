#include "LevelInGame_BG.h"

#include "Common.h"
#include "Engine/Engine.h"

// 利用モジュール
#include "Engine/Platform/PlatformModule.h"
#include "RenderModule.h"

namespace Level
{
    HE::Bool LevelInGame_BG::VBegin()
    {
        const HE::Bool bRet = Node::VBegin();
        HE_ASSERT(bRet);

        //	点の位置を複数作成
        {
            // スクリーンサイズ取得
            auto pPlatformModule = HE_ENGINE.ModuleManager().Get<Platform::PlatformModule>();
            // auto pScreen   = pPlatform->VScreen();
            auto pSystem = pPlatformModule->VSystem();

            auto pRenderModule = HE_ENGINE.ModuleManager().Get<Render::RenderModule>();
            auto pViewPort     = pRenderModule->GetViewPort(Game::g_scene2DHandle);
            // auto rScene2DEnv   = pScreen->VGetEnvBySceneView2D(Game::g_scene2DHandle);

            /*
                        this->_aPoint = reinterpret_cast<Render::Point2D*>(
                            HE_ALLOC_MEM(sizeof(Render::Point2D) * this->_uPointCount, 0));
                        for (HE::Uint32 i = 0; i < this->_uPointCount; ++i)
                        {
                            const HE::Float32 _fX = static_cast<HE::Float32>(
                                pSystem->VGetRand(pViewPort->Width()));  // rScene2DEnv._uWidth));
                            const HE::Float32 _fY = static_cast<HE::Float32>(
                                pSystem->VGetRand(pViewPort->Height()));  // rScene2DEnv._uHeight));
                            Render::Point2D* p = &this->_aPoint[i];
                            p->_fX              = _fX;
                            p->_fY              = _fY;
                            p->_color           = Core::Math::RGB::White;
                        }
                        */
            // 画面を覆う点群のパーティクルを二つ作成してスクロールさせる
            {
                Core::Common::FixedArray<Core::Math::Vector3, 1000> aPos;
                Core::Common::FixedArray<Core::Math::Vector3, 1000> aVelocity;
                Core::Common::FixedArray<Core::Math::Color, 1000> aColor;

                for (HE::Uint32 i = 0; i < 1000; ++i)
                {
                    auto x = pPlatformModule->VSystem()->VGetRandByFloat(-1.0f, 1.0f);
                    auto y = pPlatformModule->VSystem()->VGetRandByFloat(-1.0f, 1.0f);

                    Core::Math::Vector3 p(static_cast<HE::Float32>(x), static_cast<HE::Float32>(y),
                                          0.0f);
                    aPos.Set(i, p);

                    Core::Math::Vector3 v(0.0f, 0.0f, 0.0f);
                    aVelocity.Set(i, v);

                    aColor.Set(i, Core::Math::RGB::White);
                }

                {
                    auto [handle, pBlob] = pRenderModule->CreatePrticle(Game::g_scene2DHandle);
                    pBlob->Init(aPos.Capacity());

                    pBlob->SetPositions(aPos);
                    pBlob->SetVelocitys(aVelocity);
                    pBlob->SetColors(aColor);

                    this->_aParticleHandle[0] = handle;
                    this->_aParticlePos[0] =
                        Core::Math::Vector2(pViewPort->Width() >> 1, pViewPort->Height() >> 1);
                }

                {
                    auto [handle, pBlob] = pRenderModule->CreatePrticle(Game::g_scene2DHandle);
                    pBlob->Init(aPos.Capacity());

                    pBlob->SetPositions(aPos);
                    pBlob->SetVelocitys(aVelocity);
                    pBlob->SetColors(aColor);

                    this->_aParticleHandle[1] = handle;
                    this->_aParticlePos[1] =
                        Core::Math::Vector2(pViewPort->Width() + (pViewPort->Width() >> 1),
                                            pViewPort->Height() >> 1);
                }
            }
        }

        return TRUE;
    }

    HE::Bool LevelInGame_BG::VEnd()
    {
        // HE_SAFE_DELETE_MEM(this->_aPoint);
        auto pPlatformModule = HE_ENGINE.PlatformModule();
        auto pRenderModule   = HE_ENGINE.ModuleManager().Get<Render::RenderModule>();
        for (auto i = 0; i < HE_ARRAY_NUM(this->_aParticleHandle); ++i)
        {
            pRenderModule->DeletePrticle(this->_aParticleHandle[i]);
        }

        return Node::VEnd();
    }

    void LevelInGame_BG::VUpdate(const HE::Float32 in_fDt)
    {
        Node::VUpdate(in_fDt);

        // スクリーン情報を取得
        auto pPlatformModule = HE_ENGINE.PlatformModule();
        // auto pSystem         = pPlatformModule->VSystem();

        auto pRenderModule = HE_ENGINE.ModuleManager().Get<Render::RenderModule>();
        auto pViewPort     = pRenderModule->GetViewPort(Game::g_scene2DHandle);

        HE::Float32 fParticleOverflowX = -(static_cast<HE::Float32>(pViewPort->Width() >> 1));
        for (auto i = 0; i < HE_ARRAY_NUM(this->_aParticleHandle); ++i)
        {
            this->_aParticlePos[i]._fX -= 0.5f;

            if (this->_aParticlePos[i]._fX <= fParticleOverflowX)
            {
                this->_aParticlePos[i]._fX = (pViewPort->Width() + (pViewPort->Width() >> 1));
            }

            Render::Command2DParticalDraw(Game::g_scene2DHandle, this->_aParticleHandle[i],
                                          this->_aParticlePos[i]);
        }
#if 0
        // auto rScene2DEnv = pScreen->VGetEnvBySceneView2D(Game::g_scene2DHandle);

        const HE::Float32 fMaxXPos =
            static_cast<HE::Float32>(pViewPort->Width());  // rScene2DEnv._uWidth);
        // 左から右へ動かす
        for (HE::Uint32 i = 0; i < this->_uPointCount; ++i)
        {
            this->_aPoint[i]._fX -= 0.5f;
            if (this->_aPoint[i]._fX < 0)
            {
                this->_aPoint[i]._fX = fMaxXPos;
                this->_aPoint[i]._fY =
                    pSystem->VGetRand(pViewPort->Height());  // rScene2DEnv._uHeight);
            }
        }

        // 背景を黒くする
        Render::CommandClsScreen(Game::g_scene2DHandle, Core::Math::RGB::Black);

        // 点をまとめて設定して描画
        Render::Command2DPointArrayDraw(Game::g_scene2DHandle, this->_aPoint, this->_uPointCount);
#endif
    }
}  // namespace Level
