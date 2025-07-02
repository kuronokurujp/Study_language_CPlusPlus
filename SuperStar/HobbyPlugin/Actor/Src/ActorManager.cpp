#include "Actor/ActorManager.h"

#include "Actor/Actor.h"

namespace Actor
{
    ActorManager::ActorManager(
        Core::Memory::UniquePtr<ActorManagerDecoraterlnterface> in_upDecorator)
        : _upDecorator(std::move(in_upDecorator))
    {
    }

    void ActorManager::Release()
    {
        this->End();
        HE_SAFE_DELETE_UNIQUE_PTR(this->_upDecorator);
    }

#ifdef HE_ENGINE_DEBUG
    HE::Bool ActorManager::Start(const HE::Uint32 in_uActorCapacity,
                                 const HE::Uint32 in_uActorGroupMax, const char* in_szFileName,
                                 const HE::Uint32 in_uFileLine)
#else
    HE::Bool ActorManager::Start(const HE::Uint32 in_uActorCapacity,
                                 const HE::Uint32 in_uActorGroupMax)
#endif
    {
        HE_ASSERT(1 <= in_uActorGroupMax);
        const auto uGroupMax = in_uActorGroupMax + 1;

#ifdef HE_ENGINE_DEBUG
        if (this->_taskManager.Init(in_uActorCapacity, uGroupMax, in_szFileName, in_uFileLine) ==
            FALSE)
            return FALSE;
#else
        if (this->_taskManager.Init(in_uActorCapacity, uGroupMax) == FALSE) return FALSE;
#endif

        if (this->_upDecorator)
        {
            if (this->_upDecorator->VStart(this) == FALSE) return TRUE;
        }

        return TRUE;
    }

    HE::Bool ActorManager::End()
    {
        this->_taskManager.End();

        return TRUE;
    }

    /// <summary>
    /// Removes the actor.
    /// </summary>
    void ActorManager::Remove(Core::Common::Handle in_handle)
    {
        HE_ASSERT_RETURN((in_handle.Null() == FALSE) && "アクターを破棄するハンドルがない");

        // 入力コンポーネントがついている場合は登録リストから外す
        auto pObject = this->Get(in_handle);
        if (pObject != NULL)
        {
            pObject->ForeachComponents(
                [this](const Core::Common::Handle& in_rHandle, Actor::Component* in_pCmp)
                {
                    if (in_rHandle.Null()) return TRUE;

                    this->VOnActorUnRegistComponent(in_pCmp);
                    return TRUE;
                });

            this->_taskManager.RemoveTask(in_handle);
            return;
        }

        // タスクの全削除処理ですでにタスクが存在しないケースがある
        // ペンディングマップにあるかチェック
        // if (this->_mPendingActor.Contains(*in_handle))
        {
            // ペンディングマップから外す
            //   this->_mPendingActor.Erase(*in_handle);
        }
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
            const HE::Uint32 uMax = this->_GetUpdateGroupMax();
            for (HE::Uint32 i = 0; i < uMax; ++i)
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
            const HE::Uint32 uMax = this->_GetUpdateGroupMax();
            for (HE::Uint32 i = 0; i < uMax; ++i)
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
            const HE::Uint32 uMax = this->_GetUpdateGroupMax();
            for (HE::Uint32 i = 0; i < uMax; ++i)
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

    void ActorManager::ForeachActor(std::function<void(Object*)> in_func)
    {
        const HE::Uint32 uMax = this->_GetUpdateGroupMax();
        for (HE::Uint32 i = 0; i < uMax; ++i)
        {
            this->_taskManager.ForeachByGroup(i,
                                              [&in_func](Core::Task* in_pTask)
                                              {
                                                  auto pObj = reinterpret_cast<Object*>(in_pTask);
                                                  in_func(pObj);
                                              });
        }
    }

    HE::Bool ActorManager::_SetupObject(const Core::Common::Handle in_handle)
    {
        Object* pObject = this->_taskManager.GetTask<Object>(in_handle);
        HE_ASSERT(pObject != NULL);

        pObject->SetEventComponent(
            // コンポーネント登録と解除の受信
            [this](Component* in_pComp, const Object::EComponentState in_eState)
            {
                if (in_eState == Object::EComponentState::EComponentState_Regist)
                    this->VOnActorRegistComponent(in_pComp);
                else if (in_eState == Object::EComponentState::EComponentState_UnRegist)
                {
                    this->VOnActorUnRegistComponent(in_pComp);
                }
            });

        return TRUE;
    }

    void ActorManager::_UpdatePending()
    {
        // 保留グループに設定したタスクを指定グループに移動
        this->_taskManager.ForeachByGroup(this->_GetPendingGroupId(),
                                          [this](Core::Task* in_pTask)
                                          {
                                              Object* pObj = reinterpret_cast<Object*>(in_pTask);
                                              this->_taskManager.MoveGropuTask(pObj->Handle(),
                                                                               pObj->GetGropuId());
                                          });
    }
}  // namespace Actor
