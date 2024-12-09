#include "ActorManager.h"

#include "Actor.h"

namespace Actor
{
    ActorManager::ActorManager(
        Core::Memory::UniquePtr<ActorManagerDecoraterlnterface> in_upDecorator)
        : _upDecorator(std::move(in_upDecorator))
    {
    }

    HE::Bool ActorManager::Start(const HE::Uint32 in_uActorCapacity,
                                 const HE::Uint32 in_uActorGroupMax)
    {
        HE_ASSERT(1 < in_uActorGroupMax);
        if (this->_taskManager.Init(in_uActorCapacity, in_uActorGroupMax) == FALSE) return FALSE;

        if (this->_upDecorator)
        {
            if (this->_upDecorator->VStart(this) == FALSE) return TRUE;
        }

        return TRUE;
    }

    HE::Bool ActorManager::End()
    {
        HE_SAFE_DELETE_UNIQUE_PTR(this->_upDecorator);
        this->_taskManager.End();

        return TRUE;
    }

    /// <summary>
    /// Removes the actor.
    /// </summary>
    void ActorManager::Remove(Core::Common::Handle* in_pActorHandle)
    {
        HE_ASSERT(in_pActorHandle);
        HE_ASSERT((in_pActorHandle->Null() == FALSE) && "アクターを破棄するハンドルがない");

        // 入力コンポーネントがついている場合は登録リストから外す
        auto pObject = this->Get(*in_pActorHandle);

        // タスクの全削除処理ですでにタスクが存在しないケースがある
        if (pObject == NULL)
        {
            in_pActorHandle->Clear();
            return;
        }

        pObject->ForeachComponents(
            [this](const Core::Common::Handle& in_rHandle, Actor::Component* in_pCmp)
            {
                if (in_rHandle.Null()) return TRUE;

                this->VOnActorUnRegistComponent(in_pCmp);
                return TRUE;
            });

        this->_taskManager.RemoveTask(in_pActorHandle);
    }

    Object* ActorManager::Get(const Core::Common::Handle& in_rActorHandle)
    {
        HE_ASSERT((in_rActorHandle.Null() == FALSE) && "アクターを破棄するハンドルがない");
        return reinterpret_cast<Object*>(this->_taskManager.GetTask(in_rActorHandle));
    }

    void ActorManager::VOnActorRegistComponent(Component* in_pComponent)
    {
        if (this->_upDecorator == NULL) return;

        this->_upDecorator->VOnActorRegistComponent(in_pComponent);
    }

    void ActorManager::VOnActorUnRegistComponent(Component* in_pComponent)
    {
        if (this->_upDecorator == NULL) return;

        this->_upDecorator->VOnActorUnRegistComponent(in_pComponent);
    }

    void ActorManager::BeginUpdate(const HE::Float32 in_fDt)
    {
        {
            const HE::Sint32 iMax = this->_GetUpdateGroupMax();
            for (HE::Sint32 i = 0; i < iMax; ++i)
            {
                this->_taskManager.ForeachByGroup(i,
                                                  [in_fDt](Core::Task* in_pTask)
                                                  {
                                                      auto pObj =
                                                          reinterpret_cast<Object*>(in_pTask);
                                                      pObj->VBeginUpdate(in_fDt);
                                                  });
            }
        }
    }

    void ActorManager::Update(const HE::Float32 in_fDt)
    {
        // アクター処理内で新しいアクターが追加した場合は保留リストにまず登録させる
        this->_bUpdatingActors = TRUE;
        {
            const HE::Sint32 iMax = this->_GetUpdateGroupMax();
            for (HE::Sint32 i = 0; i < iMax; ++i)
            {
                this->_taskManager.UpdateByGroup(i, in_fDt);
            }
        }
        this->_bUpdatingActors = FALSE;
    }

    void ActorManager::LateUpdate(const HE::Float32 in_fDt)
    {
        this->_UpdatePending();

        {
            const HE::Sint32 iMax = this->_GetUpdateGroupMax();
            for (HE::Sint32 i = 0; i < iMax; ++i)
            {
                this->_taskManager.ForeachByGroup(i,
                                                  [in_fDt](Core::Task* in_pTask)
                                                  {
                                                      auto pObj =
                                                          reinterpret_cast<Object*>(in_pTask);
                                                      pObj->VLateUpdate(in_fDt);
                                                  });
            }
        }
    }

    void ActorManager::Event(const Core::TaskData& in_rTaskData)
    {
        this->_taskManager.EventAll(in_rTaskData);
    }

    void ActorManager::_UpdatePending()
    {
        // 保留グループに設定したタスクを更新グループに移動
        for (auto b = this->_pendingDataMap.Begin(); b != this->_pendingDataMap.End(); ++b)
        {
            const auto pData = &b->data;
            const HE::Bool bRet =
                this->_taskManager.MoveGropuTask(pData->handle, pData->sMoveGroupId);
            HE_ASSERT(bRet && "保留中のタスクを稼働中に切り替え失敗した");
        }
        this->_pendingDataMap.Clear();
    }

}  // namespace Actor
