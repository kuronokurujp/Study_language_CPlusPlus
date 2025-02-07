#include "InGameZakoEnemyActor.h"

#include "InGame/Component/InGameCollisionComponent.h"
#include "InGame/Component/Renderer/InGameRendererEnemyZakoComponent.h"
#include "InGame/InGameTag.h"

// エンジン
#include "Engine/Engine.h"

// モジュール
#include "LuaModule.h"

namespace InGame
{
    InGameEnemyZakoActor::InGameEnemyZakoActor(const Core::Common::Handle in_luaObjectHandle,
                                               const Parameter& in_rParameter)
        : InGameScene2DActor(), _luaObjectHandle(in_luaObjectHandle), _parameter(in_rParameter)
    {
        this->_Clear();
    }

    HE::Bool InGameEnemyZakoActor::VBegin()
    {
        if (InGameScene2DActor::VBegin() == FALSE) return FALSE;

        // 当たり判定コンポーネント追加
        {
            auto [handle, pComponent] =
                this->AddComponentByHandleAndComp<InGameCircleCollision2DComponent>(
                    0, Actor::Component::EPriorty::EPriorty_Main);
            HE_ASSERT(handle.Null() == FALSE);

            pComponent->SetRadius(HE_MIN(this->_size._fX, this->_size._fY));
            pComponent->SetCollisionHashCode(InGame::EObjectTag::EObjectTag_Enemy);
            pComponent->SetMetaData(this->Handle());
        }

        // Luaスクリプトで敵の挙動制御をする
        {
            auto pTrans = this->GetComponent<Actor::TransformComponent>(this->_transformHandle);
            HE_ASSERT(pTrans);

            auto pos  = pTrans->GetWorldPos();
            auto rDir = pTrans->GetDir();

            auto pModule = HE_ENGINE.ModuleManager().Get<Lua::LuaModule>();
            (pModule->CallScriptFunc(this->_luaObjectHandle, HE_STR_TEXT("EnemyEvent_Init"),
                                     this->Handle(), pos._fX, pos._fY, rDir._fX, rDir._fY,
                                     this->_parameter.speed));
        }

        this->_bScreenInSide = TRUE;

        return TRUE;
    }

    HE::Bool InGameEnemyZakoActor::VEnd()
    {
        auto bRet = InGameScene2DActor::VEnd();
        // Luaスクリプトの終了通知
        {
            auto pModule = HE_ENGINE.ModuleManager().Get<Lua::LuaModule>();
            (pModule->CallScriptFunc(this->_luaObjectHandle, HE_STR_TEXT("EnemyEvent_End"),
                                     this->Handle()));

            // Luaスクリプトを削除
            pModule->ReleaseLuaObject(this->_luaObjectHandle);
        }

        return bRet;
    }

    void InGameEnemyZakoActor::VUpdate(const HE::Float32 in_fDt)
    {
        InGameScene2DActor::VUpdate(in_fDt);

        // 移動情報による座標更新
        {
            Core::Math::Vector2 newPos;
            newPos.SetAdd(this->_pos, this->_move);
            this->_move.Clear();

            this->SetPos(newPos);
        }

        auto pModule = HE_ENGINE.ModuleManager().Get<Lua::LuaModule>();
        // Luaスクリプトの更新
        {
            pModule->CallScriptFunc(this->_luaObjectHandle, HE_STR_TEXT("EnemyEvent_Update"),
                                    this->Handle(), in_fDt);
        }

        // 画面外かどうかチェック
        if (this->_bScreenInSide)
        {
            auto pRendererComponent = this->GetComponent<InGameRendererEnemyZakoComponent>();
            if (pRendererComponent && (pRendererComponent->IsScreenIn()))
            {
                // 画面内
            }
            else
            {
                // 画面外
                this->_bScreenInSide = FALSE;
                // 画面外の処理になったらどうするか個々のキャラによって異なる
                pModule->CallScriptFunc(this->_luaObjectHandle, HE_STR_TEXT("EnemyEvent_ScreenOut"),
                                        this->Handle());
            }
        }
    }

    void InGameEnemyZakoActor::SetSize(const Core::Math::Vector2& in_rSize)
    {
        this->_size             = in_rSize;
        auto pRendererComponent = this->GetComponent<InGameRendererEnemyZakoComponent>();
        if (pRendererComponent)
        {
            pRendererComponent->VSetSize(in_rSize);
        }
    }

    void InGameEnemyZakoActor::Damage(const HE::Sint32 in_sDamage)
    {
        auto pModule = HE_ENGINE.ModuleManager().Get<Lua::LuaModule>();
        HE_ASSERT(pModule);

        pModule->CallScriptFunc(this->_luaObjectHandle, HE_STR_TEXT("EnemyEvent_Damage"),
                                this->Handle(), in_sDamage);
    }

    void InGameEnemyZakoActor::_Clear()
    {
    }

#if 0
    /*
            @brief	アクション通常
    */
    void C_EnemyActorZako::_VNormalAct()
    {
        m_Vec = m_DirVec;
        m_Vec.Scale(m_Speed);
        m_pos += m_Vec;

        if (m_NextStateCnt > 0)
        {
            if ((--m_NextStateCnt) <= 0.f)
            {
                //	次のステートをスクリプト側で決めてもらう
                //	パラメータを作成
                EnemyZakoNormalParamEventData eventData(VGetHandle());
                eventData.m_Angle = atan2(m_DirVec.y, m_DirVec.x) * RADIAN_TO_ANGLE;
                eventData.m_Hp    = m_Hp;
                eventData.m_Speed = m_Speed;
                LuaStateManager::DoFile(s_aEnemyZakoScriptFileName[m_eAblityType], "EndNormalAct",
                                        &eventData);
            }
        }
    }

    /*
            @brief
            @note	逃げる
    */
    void C_EnemyActorZako::_VEscapeAct()
    {
        Vec3 vec = m_DirVec;
        vec *= m_Speed;
        m_pos.Add(vec);
    }

    /*
            @brief	死亡
    */
    void C_EnemyActorZako::_VDieAct()
    {
        SystemProprtyInterfaceInGame::AddPoint(m_Point);
    }

#endif
}  // namespace InGame
