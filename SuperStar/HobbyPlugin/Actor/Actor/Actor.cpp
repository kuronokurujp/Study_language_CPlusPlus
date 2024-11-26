#include "Actor.h"

#include "Actor/ActorManager.h"
#include "Actor/Component/Component.h"

namespace Actor
{
    Object::Object() : TaskTree()
    {
        if (this->_components.Init(32, 32) == FALSE)
        {
            HE_ASSERT(FALSE && "コンポーネントのタスク管理の初期化失敗");
        }

        if (this->_lateComponents.Init(32, 32) == FALSE)
        {
            HE_ASSERT(FALSE && "コンポーネントのタスク管理の初期化失敗");
        }
    }

    Object::~Object()
    {
        this->_components.End();
        this->_lateComponents.End();
        this->_pOwner = NULL;
    }

    void Object::SetOwner(ActorManager* in_pOwner)
    {
        HE_ASSERT(in_pOwner);
        this->_pOwner = in_pOwner;
    }

    Object* Object::GetActorByOwner(const Core::Common::Handle& in_rHandle)
    {
        HE_ASSERT(this->_pOwner);
        return this->_pOwner->Get(in_rHandle);
    }

    void Object::VSetup(const Bool in_bAutoDelete)
    {
        TaskTree::VSetup(in_bAutoDelete);

        this->_Clear();
    }

    Bool Object::VBegin()
    {
        return TaskTree::VBegin();
    }

    Bool Object::VEnd()
    {
        // 設定しているコンポーネントを全て破棄
        this->RemoveAllComponent();

        // コンポーネント終了
        this->_components.End();
        this->_lateComponents.End();

        return TaskTree::VEnd();
    }

    void Object::VBeginUpdate(const Float32 in_fDt)
    {
        if (this->_eState != EState_Active) return;

        this->ForeachChildTask(
            [in_fDt](Core::Task* in_pChildTask)
            {
                auto pObject = reinterpret_cast<Object*>(in_pChildTask);
                pObject->VBeginUpdate(in_fDt);
            });
    }

    void Object::VUpdate(const Float32 in_fDt)
    {
        if (this->_eState != EState_Active) return;

        // コンポーネント更新
        this->_components.UpdateAll(in_fDt);

        // 子タスクの実行をしているので処理が全て終わった後に実行
        Core::TaskTree::VUpdate(in_fDt);
    }

    void Object::VLateUpdate(const Float32 in_fDt)
    {
        if (this->_eState != EState_Active) return;

        // コンポーネント更新
        this->_lateComponents.UpdateAll(in_fDt);

        this->ForeachChildTask(
            [in_fDt](Core::Task* in_pChildTask)
            {
                auto pObject = reinterpret_cast<Object*>(in_pChildTask);
                pObject->VLateUpdate(in_fDt);
            });
    }

    /// <summary>
    /// くっつている全てのコンポーネント削除.
    /// </summary>
    void Object::RemoveAllComponent()
    {
        this->_RemoveAllComponent(&this->_components);
        this->_RemoveAllComponent(&this->_lateComponents);
    }

    Bool Object::RemoveComponent(Core::Common::Handle* in_pHandle)
    {
        if (this->_RemoveComponent(in_pHandle, &this->_components))
        {
            return TRUE;
        }

        if (this->_RemoveComponent(in_pHandle, &this->_lateComponents))
        {
            return TRUE;
        }

        return FALSE;
    }

    /// <summary>
    /// アクター自身, 子アクターに設定しているコンポーネントを全て出力
    /// </summary>
    void Object::OutputChildrenComponent(Core::Common::StackBase<Component*>* out,
                                         const Core::Common::RTTI* in_pRtti)
    {
        auto [h, c] = this->GetComponentHandleAndComponent(in_pRtti);
        if (h.Null() == FALSE)
        {
            out->PushBack(c);
        }

        if (this->_lstChildTask.Empty()) return;
        auto itr    = this->_lstChildTask.BeginItr();
        auto endItr = this->_lstChildTask.EndItr();

        Core::Common::FixedStack<Core::Common::CustomList<ChildTaskNode>::Iterator, 32>
            listItrStack;

        while (itr != endItr)
        {
            auto pChildActor = this->_pTaskManager->GetTask<Object>(itr->GetHandle());
            HE_ASSERT(pChildActor);

            pChildActor->ForeachComponents(
                [in_pRtti, out](const Core::Common::Handle& in_rHandle, Actor::Component* in_pCmp)
                {
                    if (in_pCmp->VGetRTTI().DerivesFrom(in_pRtti))
                    {
                        out->PushBack(in_pCmp);
                    }

                    return TRUE;
                });
            /*
                        auto slot = pChildActor->_components.GetUserDataList();
                        for (auto b = slot->begin(); b != slot->end(); ++b)
                        {
                            auto c = reinterpret_cast<Actor::Component*>(b->second);
                            if (c->VGetRTTI().DerivesFrom(in_pRtti))
                            {
                                out->PushBack(c);
                            }
                        }
                        */

            ++itr;

            // タスクに子タスクがあれば,処理するタスクリストを子タスクのに変える
            // リスト精査が終わったら,タスクリストを親タスクのリストに戻すので更新前のイテレータをスタックで保存しておく
            if (pChildActor->_lstChildTask.Empty() == FALSE)
            {
                listItrStack.PushBack(itr);
                listItrStack.PushBack(endItr);

                itr    = pChildActor->_lstChildTask.BeginItr();
                endItr = pChildActor->_lstChildTask.EndItr();
            }
            else
            {
                // リストの精査が終わった
                if (itr == endItr)
                {
                    // スタックに保存したイテレータがある場合は
                    // タスクリストを親タスクのリストに戻す
                    if (listItrStack.Empty() == FALSE)
                    {
                        endItr = listItrStack.PopBack();
                        itr    = listItrStack.PopBack();
                    }
                }
            }
        }
    }

    std::tuple<Core::Common::Handle, Actor::Component*> Object::GetComponentHandleAndComponent(
        const Core::Common::RTTI* in_pRtti)
    {
        /*
            auto list = this->GetComponents();
            auto end  = list->end();
            for (auto itr = list->begin(); itr != end; ++itr)
            {
                Component* target = reinterpret_cast<Component*>(itr->second);
                if (target->VGetRTTI().DerivesFrom(in_pRtti)) return itr->first;
            }
            */
        Core::Common::Handle handle;
        Actor::Component* pCmp = NULL;
        this->ForeachComponents(
            [in_pRtti, &handle, &pCmp](const Core::Common::Handle& in_rHandle, Component* in_pCmp)
            {
                if (in_pCmp->VGetRTTI().DerivesFrom(in_pRtti))
                {
                    handle = in_rHandle;
                    pCmp   = in_pCmp;
                    return FALSE;
                }

                return TRUE;
            });

        return std::tuple<Core::Common::Handle, Actor::Component*>(handle, pCmp);
    }

    void Object::ForeachComponents(
        std::function<Bool(const Core::Common::Handle&, Component*)> in_func)
    {
        auto list = this->_components.GetUserDataList();
        auto end  = list->end();
        for (auto itr = list->begin(); itr != end; ++itr)
        {
            if (in_func(itr->first, reinterpret_cast<Component*>(itr->second)) == FALSE) break;
        }

        list = this->_lateComponents.GetUserDataList();
        end  = list->end();
        for (auto itr = list->begin(); itr != end; ++itr)
        {
            if (in_func(itr->first, reinterpret_cast<Component*>(itr->second)) == FALSE) break;
        }
    }

    Bool Object::_VSetupComponent(Component* in_pComp)
    {
        HE_ASSERT(in_pComp);

        // コンポーネントを付けた自身を設定
        in_pComp->SetOwner(this);
        this->_pOwner->VOnActorRegistComponent(in_pComp);

        return TRUE;
    }

    void Object::_RemoveAllComponent(Core::TaskManager* in_pComponents)
    {
        // コンポーネント解除をオーナーに通知
        auto components = in_pComponents->GetUserDataList();
        for (auto itr = components->begin(); itr != components->end(); ++itr)
        {
            auto pComponent = reinterpret_cast<Component*>(itr->second);
            this->_pOwner->VOnActorUnRegistComponent(pComponent);
        }

        in_pComponents->RemoveAll();
    }

    Bool Object::_RemoveComponent(Core::Common::Handle* in_pHandle,
                                  Core::TaskManager* in_pComponents)
    {
        HE_ASSERT(in_pHandle);
        HE_ASSERT(in_pHandle->Null() == FALSE);

        // コンポーネント解除をオーナーに通知
        auto pComponent = reinterpret_cast<Component*>(in_pComponents->GetTask(*in_pHandle));
        if (pComponent == NULL)
        {
            in_pHandle->Null();
            return FALSE;
        }

        this->_pOwner->VOnActorUnRegistComponent(pComponent);

        in_pComponents->RemoveTask(in_pHandle);

        return TRUE;
    }

#if 0
    /// <summary>
    /// Sets the rotation deg.
    /// 時計周りがマイナス値、反時計周りがプラス値
    /// </summary>
    /// <param name="in_rAngle">The in r angle.</param>
    /// <returns></returns>
    auto Actor::SetRotationAngleUnitDeg(const Math::Vector3 in_rAngle) -> void
    {
        // 角度からクォータニオンを作成
        auto angle = Math::Vector3(in_rAngle);
        angle.Mul(Math::RADIANS_UNIT);

        this->rotation.AngleUnitRadian(angle);

        this->recomputeWorldTransform = true;
    }

    /// <summary>
    /// Gets the forward.
    /// </summary>
    /// <returns></returns>
    auto Actor::GetForward() const -> Math::Vector3
    {
        return Math::Vector3Transform(Math::Vector3::Unit_X, this->rotation);
    }

#endif
}  // namespace Actor
