#include "LevelInGame_BG.h"

// 利用モジュール
#include "Engine/Platform/PlatformModule.h"
#include "RenderModule.h"

namespace Level
{
    Bool LevelInGame_BG::VBegin()
    {
        const Bool bRet = Node::VBegin();
        HE_ASSERT(bRet);

        // レンダリングビュー作成
        {
            // 一番奥にビュー追加
            auto pRenderModule = HE_ENGINE.ModuleManager().Get<Render::RenderModule>();
            this->_viewHandle  = pRenderModule->AddView(0);
        }

        //	点の位置を複数作成
        {
            // スクリーンサイズ取得
            auto pPlatform = HE_ENGINE.ModuleManager().Get<Platform::PlatformModule>();
            auto pScreen   = pPlatform->VScreen();
            auto pSystem   = pPlatform->VSystem();

            Uint32 uW     = pScreen->VWidth();
            Uint32 uH     = pScreen->VHeight();
            this->_aPoint = HE_NEW_MEM_ARRAY(Render::Point2D, this->_uPointCount, 0);
            for (Uint32 i = 0; i < this->_uPointCount; ++i)
            {
                const Float32 fX   = static_cast<Float32>(pSystem->VGetRand(uW));
                const Float32 fY   = static_cast<Float32>(pSystem->VGetRand(uH));
                Render::Point2D* p = &this->_aPoint[i];
                p->fX              = fX;
                p->fY              = fY;
                p->color           = Render::RGB::White;
            }
        }

        return TRUE;
    }

    Bool LevelInGame_BG::VEnd()
    {
        HE_SAFE_DELETE_MEM_ARRAY(this->_aPoint);

        auto pRenderModule = HE_ENGINE.ModuleManager().Get<Render::RenderModule>();
        pRenderModule->RemoveView(this->_viewHandle);

        return Node::VEnd();
    }

    void LevelInGame_BG::VUpdate(const Float32 in_fDt)
    {
        Node::VUpdate(in_fDt);

        // スクリーン情報を取得
        auto pPlatform = HE_ENGINE.ModuleManager().Get<Platform::PlatformModule>();
        auto pScreen   = pPlatform->VScreen();
        auto pSystem   = pPlatform->VSystem();

        const Uint32 uW = pScreen->VWidth();
        const Uint32 uH = pScreen->VHeight();

        const Float32 fMaxXPos = static_cast<Float32>(uW);
        // 左から右へ動かす
        for (Uint32 i = 0; i < this->_uPointCount; ++i)
        {
            this->_aPoint[i].fX -= 0.5f;
            if (this->_aPoint[i].fX < 0)
            {
                this->_aPoint[i].fX = fMaxXPos;
                this->_aPoint[i].fY = pSystem->VGetRand(uH);
            }
        }

        // 背景を黒くする
        Render::CommandClsScreen(this->_viewHandle, Render::RGB::Black);

        // 点をまとめて設定して描画
        Render::Command2DPointArrayDraw(this->_viewHandle, this->_aPoint, this->_uPointCount);
    }
}  // namespace Level
