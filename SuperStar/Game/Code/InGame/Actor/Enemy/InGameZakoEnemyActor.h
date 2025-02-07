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
        /// <summary>
        /// 自機のパラメータ
        /// </summary>
        struct Parameter
        {
            // 移動スピード
            HE::Float32 speed = 5.0f;

            // HP
            HE::Sint32 ulife = 3;
        };

    public:
        InGameEnemyZakoActor(const Core::Common::Handle in_luaObject,
                             const Parameter& in_rParameter);

        HE::Bool VBegin() override final;
        HE::Bool VEnd() override final;

        void VUpdate(const HE::Float32 in_fDt) override final;

        void SetSize(const Core::Math::Vector2& in_rSize);

        void Move(const Core::Math::Vector2& in_rMove)
        {
            this->_move.Clear();
            this->_move.Madd(in_rMove, this->_parameter.speed);
        }

        void Damage(const HE::Sint32 in_sDamage);

        // virtual bool update(void);
        // virtual void draw(void);

        //	データ通知
        // virtual void VOnCallbackSendMessage(const int in_DataType, void* in_pData);

    private:
        void _Clear();
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
        Core::Math::Vector2 _size;
        Core::Common::Handle _luaObjectHandle;
        Parameter _parameter;
        HE::Bool _bScreenInSide = TRUE;
        Core::Math::Vector2 _move;
    };
}  // namespace InGame
