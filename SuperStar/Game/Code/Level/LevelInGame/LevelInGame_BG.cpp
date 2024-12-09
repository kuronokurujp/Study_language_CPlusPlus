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
            auto pPlatform = HE_ENGINE.ModuleManager().Get<Platform::PlatformModule>();
            auto pScreen   = pPlatform->VScreen();
            auto pSystem   = pPlatform->VSystem();

            auto rScene2DEnv = pScreen->GetEnvBySceneView2D(Game::g_scene2DHandle);

            this->_aPoint = reinterpret_cast<Render::Point2D*>(
                HE_ALLOC_MEM(sizeof(Render::Point2D) * this->_uPointCount, 0));
            for (HE::Uint32 i = 0; i < this->_uPointCount; ++i)
            {
                const HE::Float32 fX =
                    static_cast<HE::Float32>(pSystem->VGetRand(rScene2DEnv._uWidth));
                const HE::Float32 fY =
                    static_cast<HE::Float32>(pSystem->VGetRand(rScene2DEnv._uHeight));
                Render::Point2D* p = &this->_aPoint[i];
                p->fX              = fX;
                p->fY              = fY;
                p->color           = Render::RGB::White;
            }
        }

        return TRUE;
    }

    HE::Bool LevelInGame_BG::VEnd()
    {
        HE_SAFE_DELETE_MEM(this->_aPoint);

        return Node::VEnd();
    }

    void LevelInGame_BG::VUpdate(const HE::Float32 in_fDt)
    {
        Node::VUpdate(in_fDt);

        // スクリーン情報を取得
        auto pPlatform = HE_ENGINE.ModuleManager().Get<Platform::PlatformModule>();
        auto pScreen   = pPlatform->VScreen();
        auto pSystem   = pPlatform->VSystem();

        auto rScene2DEnv = pScreen->GetEnvBySceneView2D(Game::g_scene2DHandle);

        const HE::Float32 fMaxXPos = static_cast<HE::Float32>(rScene2DEnv._uWidth);
        // 左から右へ動かす
        for (HE::Uint32 i = 0; i < this->_uPointCount; ++i)
        {
            this->_aPoint[i].fX -= 0.5f;
            if (this->_aPoint[i].fX < 0)
            {
                this->_aPoint[i].fX = fMaxXPos;
                this->_aPoint[i].fY = pSystem->VGetRand(rScene2DEnv._uHeight);
            }
        }

        // 背景を黒くする
        Render::CommandClsScreen(Game::g_scene2DHandle, Render::RGB::Black);

        // 点をまとめて設定して描画
        Render::Command2DPointArrayDraw(Game::g_scene2DHandle, this->_aPoint, this->_uPointCount);
    }
}  // namespace Level
