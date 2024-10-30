#pragma once

#include "InGame/Actor/InGameScene2DActor.h"

// エンジンの最小インクルード
#include "Engine/MiniEngine.h"

// 利用モジュール
#include "ActorModule.h"

namespace InGame
{
    /// <summary>
    /// 雑魚敵
    /// </summary>
    class InGameEnemyZakoActor final : public InGameScene2DActor
    {
        HE_GENERATED_CLASS_BODY_HEADER(InGameEnemyZakoActor, InGameScene2DActor);

    public:
        InGameEnemyZakoActor();
        virtual ~InGameEnemyZakoActor();

        Bool VBegin() override final;

        void VUpdate(const Float32 in_fDt) override final;

        // virtual bool update(void);
        // virtual void draw(void);

        //	データ通知
        // virtual void VOnCallbackSendMessage(const int in_DataType, void* in_pData);

    private:
        //	関数
        void _Clear() { this->_fSpeed = 0.0f; }
        /*
                virtual void _VOnCallbackOtherActorData(C_PaketSendActor* in_pOtherActor);
                //	スクリプトで通知したデータを受け取る
                virtual void _VOnCallbackScriptEvent(lua_State* in_pLuaState);

                //	アクション
                virtual void _VNormalAct();
                virtual void _VEscapeAct();
                virtual void _VAttackAct() {}

                virtual void _VDieAct();
                virtual void _VDamageAct() {}

                //	変数
                float m_Speed;
                //Vec3 m_DirVec;
                //Vec3 m_Vec;
                */
    private:
        Float32 _fSpeed = 0.0f;
    };
}  // namespace InGame
