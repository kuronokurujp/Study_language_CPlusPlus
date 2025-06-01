#include "LevelNode.h"

#include "Level/Component/LevelBaseComponent.h"

// 利用モジュール一覧
#include "EnhancedInputModule.h"

namespace Level
{
    Node::Node()
        : Actor::Object(),
          _actorManager(
              HE_MAKE_CUSTOM_UNIQUE_PTR((EnhancedInput::ActorManagerDecoraterWithInputSystem)))
    {
    }

    HE::Bool Node::VBegin()
    {
        if (Actor::Object::VBegin() == FALSE) return FALSE;

        if (ACTOR_MANAGER_START(this->_actorManager, 256, 2) == FALSE)
        {
            HE_ASSERT(FALSE && "レベルノードのアクター管理の初期化が失敗");
            return FALSE;
        }

        return TRUE;
    }

    HE::Bool Node::VEnd()
    {
        if (this->_actorManager.End() == FALSE)
        {
            HE_ASSERT(FALSE && "レベルノードの終了に失敗");
            return FALSE;
        }

        return Actor::Object::VEnd();
    }

    void Node::VBeginUpdate(const HE::Float32 in_fDt)
    {
        Actor::Object::VBeginUpdate(in_fDt);
        this->_actorManager.BeginUpdate(in_fDt);
    }

    void Node::VUpdate(const HE::Float32 in_fDt)
    {
        Actor::Object::VUpdate(in_fDt);

        this->_actorManager.Update(in_fDt);
    }

    void Node::VLateUpdate(const HE::Float32 in_fDt)
    {
        Actor::Object::VLateUpdate(in_fDt);
        this->_actorManager.LateUpdate(in_fDt);
    }

    void Node::VProcessInput(const EnhancedInput::InputMap& in_pInputMap)
    {
        auto pDecotrater = reinterpret_cast<EnhancedInput::ActorManagerDecoraterWithInputSystem*>(
            this->_actorManager.GetDecorater());
        pDecotrater->ProcessInput(in_pInputMap);
    }
    /*
        void Node::VEvent(const Core::TaskData& in_rTaskData)
        {
            Actor::Object::VEvent(in_rTaskData);

            switch (in_rTaskData.uId)
            {
                // 入力送信
                case Node::ETaskUpdateId_Input:
                {
                    HE_ASSERT(in_rTaskData.pData);
                    const auto pInputMap =
                        reinterpret_cast<EnhancedInput::InputMap*>(in_rTaskData.pData);

                    this->_VProcessInput(pInputMap);

                    break;
                }
            }
        }
        */
    // レベルに追加されたアクターを削除
    void Node::RemoveActor(Core::Common::Handle* in_pActor)
    {
        HE_ASSERT(in_pActor);
        if (in_pActor->Null()) return;

        this->_actorManager.Remove(in_pActor);
    }

    HE::Bool Node::ChainActor(const Core::Common::Handle& in_rChildActor,
                              const Core::Common::Handle& in_rParentActor)
    {
        HE_ASSERT(in_rChildActor.Null() == FALSE);
        HE_ASSERT(in_rParentActor.Null() == FALSE);

        auto pParentActor = this->GetActor<Actor::Object>(in_rParentActor);
        // 子アクターに親アクターを設定する
        return pParentActor->AddChildTask(in_rChildActor);
    }

    void Node::_VDestory()
    {
        this->_actorManager.Release();
        Actor::Object::_VDestory();
    }

    /// <summary>
    /// 追加したコンポーネントのセットアップ
    /// </summary>
    HE::Bool Node::_VSetupComponent(Actor::Component* in_pComp)
    {
        HE_ASSERT(in_pComp);

        if (HE_GENERATED_CHECK_RTTI((*in_pComp), LevelBaseComponent) == FALSE)
        {
            HE_ASSERT(0 && "レベル専用のコンポーネントではない");
            return FALSE;
        }

        return Actor::Object::_VSetupComponent(in_pComp);
    }

}  // namespace Level
