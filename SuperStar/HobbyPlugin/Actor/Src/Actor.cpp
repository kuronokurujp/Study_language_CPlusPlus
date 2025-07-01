#include "Actor/Actor.h"

namespace Actor
{
    Object::Object(const HE::Uint32 in_uGroupId) : TaskTree()
    {
        this->_Clear();

        this->_uGroupId = in_uGroupId;
    }

    Object::~Object()
    {
        this->_components.End();
        this->_lateComponents.End();
        this->_componentFunction = NULL;
    }

    void Object::VSetup(const HE::Bool in_bAutoDelete)
    {
        TaskTree::VSetup(in_bAutoDelete);

        if (TASK_MANAGER_INIT(this->_components, 32, 32) == FALSE)
        {
            HE_ASSERT(FALSE && "コンポーネントのタスク管理の初期化失敗");
        }

        if (TASK_MANAGER_INIT(this->_lateComponents, 32, 32) == FALSE)
        {
            HE_ASSERT(FALSE && "コンポーネントのタスク管理の初期化失敗");
        }
    }

    HE::Bool Object::VBegin()
    {
        return TaskTree::VBegin();
    }

    HE::Bool Object::VEnd()
    {
        // 設定しているコンポーネントを全て破棄
        this->RemoveAllComponent();

        return TaskTree::VEnd();
    }

    void Object::VBeginUpdate(const HE::Float32 in_fDt)
    {
        if (this->_eState != EState_Active) return;

        this->ForeachChildTask(
            [in_fDt](Core::Task* in_pChildTask)
            {
                auto pObject = reinterpret_cast<Object*>(in_pChildTask);
                pObject->VBeginUpdate(in_fDt);
            });
    }

    void Object::VUpdate(const HE::Float32 in_fDt)
    {
        if (this->_eState != EState_Active) return;

        // コンポーネント更新
        this->_components.UpdateAll(in_fDt);

        // 子タスクの実行をしているので処理が全て終わった後に実行
        Core::TaskTree::VUpdate(in_fDt);
    }

    void Object::VLateUpdate(const HE::Float32 in_fDt)
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

    HE::Bool Object::RemoveComponent(Core::Common::Handle in_handle)
    {
        if (this->_RemoveComponent(in_handle, &this->_components))
        {
            return TRUE;
        }

        if (this->_RemoveComponent(in_handle, &this->_lateComponents))
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

        Core::Common::FixedStack<Core::Common::CustomList<ChildTaskNode>::IteratorChar, 32>
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
        std::function<HE::Bool(const Core::Common::Handle&, Component*)> in_func)
    {
        auto list = this->_components.GetUserDataList();
        HE_ASSERT_RETURN(list);

        auto end = list->end();
        for (auto itr = list->begin(); itr != end; ++itr)
        {
            if (in_func(itr->first, reinterpret_cast<Component*>(itr->second)) == FALSE) break;
        }

        list = this->_lateComponents.GetUserDataList();
        HE_ASSERT_RETURN(list);

        end = list->end();
        for (auto itr = list->begin(); itr != end; ++itr)
        {
            if (in_func(itr->first, reinterpret_cast<Component*>(itr->second)) == FALSE) break;
        }
    }

    void Object::_VDestory()
    {
        // 設定しているコンポーネントを全て破棄
        this->RemoveAllComponent();

        // コンポーネント終了
        this->_components.End();
        this->_lateComponents.End();

        TaskTree::_VDestory();
    }

    HE::Bool Object::_VSetupComponent(Component* in_pComp)
    {
        HE_ASSERT(in_pComp);

        // コンポーネントを付けた自身を設定
        in_pComp->SetOwner(this);
        if (this->_componentFunction != NULL)
            this->_componentFunction(in_pComp, EComponentState::EComponentState_Regist);

        return TRUE;
    }

    void Object::_RemoveAllComponent(Core::TaskManager* in_pComponents)
    {
        // コンポーネント解除をオーナーに通知
        auto components = in_pComponents->GetUserDataList();
        if (components && this->_componentFunction != NULL)
        {
            for (auto itr = components->begin(); itr != components->end(); ++itr)
            {
                auto pComponent = reinterpret_cast<Component*>(itr->second);
                this->_componentFunction(pComponent, EComponentState::EComponentState_UnRegist);
            }
        }

        in_pComponents->RemoveAll();
    }

    HE::Bool Object::_RemoveComponent(Core::Common::Handle in_handle,
                                      Core::TaskManager* in_pComponents)
    {
        HE_ASSERT(in_handle.Null() == FALSE);

        // コンポーネント解除をオーナーに通知
        auto pComponent = reinterpret_cast<Component*>(in_pComponents->GetTask(in_handle));
        if (pComponent == NULL)
        {
            return FALSE;
        }

        if (this->_componentFunction != NULL)
            this->_componentFunction(pComponent, EComponentState::EComponentState_UnRegist);

        in_pComponents->RemoveTask(in_handle);

        return TRUE;
    }
}  // namespace Actor
