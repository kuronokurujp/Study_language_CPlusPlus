#include "LevelModule.h"

#include "Engine/Memory/Memory.h"

// モジュール
#include "EnhancedInputModule.h"
#include "EventModule.h"

namespace Level
{
    LevelModule::LevelModule() : ModuleBase(ModuleName())
    {
        // 依存しているモジュールを設定
        // モジュールの依存設定していないと依存したモジュールが使えない
        this->_AppendDependenceModule<Actor::ActorModule>();
        this->_AppendDependenceModule<Platform::PlatformModule>();
        this->_AppendDependenceModule<Event::EventModule>();
        this->_AppendDependenceModule<EnhancedInput::EnhancedInputModule>();
    }

    Node& LevelModule::GetLevel(const Core::Common::Handle& in_rHandle) const
    {
        Level::Node* pNode = reinterpret_cast<Level::Node*>(this->_upNodeManager->Get(in_rHandle));
        return *pNode;
    }

    /// <summary>
    /// モジュール初期化
    /// </summary>
    HE::Bool LevelModule::_VStart()
    {
        // レベル関連の準備
        {
            // タスク管理を初期化
            // 利用するタスク設定は内部で固定する
            auto upStg =
                HE_MAKE_CUSTOM_UNIQUE_PTR((EnhancedInput::ActorManagerDecoraterWithInputSystem));

            this->_upNodeManager =
                HE_MAKE_CUSTOM_UNIQUE_PTR((Actor::ActorManager), std::move(upStg));
            if (ACTOR_MANAGER_START(*this->_upNodeManager, 32, 2) == FALSE) return FALSE;
        }

        // TODO: インプットのイベントリスナー登録
        {
            auto pEventModule      = this->GetDependenceModule<Event::EventModule>();
            auto spUIEventListener = HE_MAKE_CUSTOM_SHARED_PTR(
                (Event::EventListenerWithRegistEventFunc), HE_STR_TEXT("InputEvent"),
                [this](Event::EventDataInterfacePtr const& in_spEventData)
                {
                    // TODO: 入力イベント
                    if (in_spEventData->VEventHash() == EnhancedInput::EventInput::Hash())
                    {
                        auto pEvent =
                            reinterpret_cast<EnhancedInput::EventInput*>(in_spEventData.get());
                        this->_upNodeManager->ForeachActor(
                            [pEvent](Actor::Object* in_pObj)
                            {
                                auto pNode = reinterpret_cast<Node*>(in_pObj);
                                pNode->VProcessInput(pEvent->_mInputAction);
                            });
                    }

                    return TRUE;
                });

            this->_inputEventListenerHandle =
                pEventModule->AddListener(spUIEventListener, EVENT_TYPE_ENHANCEDINPUT);
        }
        return TRUE;
    }

    HE::Bool LevelModule::_VRelease()
    {
        {
            auto pEventModule = this->GetDependenceModule<Event::EventModule>();
            pEventModule->RemoveListener(this->_inputEventListenerHandle);
        }

        if (this->_upNodeManager->End() == FALSE) return FALSE;
        this->_upNodeManager->Release();

        HE_SAFE_DELETE_UNIQUE_PTR(this->_upNodeManager);

        return TRUE;
    }

    void LevelModule::_VBeforeUpdate(const HE::Float32 in_fDt)
    {
        // 前更新でメインレベルの切り替えなどしている
        //        this->_spLevelManager->BeforeUpdate(in_fDt);
        // レベルの切り替え先があればカレントと交換
        // 切り替えた後は古いカレントは破棄
        if ((this->_nextLevelHandle.Null() == FALSE))
        {
            auto pNode = reinterpret_cast<Node*>(this->_upNodeManager->Get(this->_nextLevelHandle));
            HE_ASSERT(pNode);
            pNode->SetState(Actor::Object::EState::EState_Active);

            if (this->_currentLevelHandle.Null() == FALSE)
            {
                this->_upNodeManager->Remove(this->_currentLevelHandle);
                this->_currentLevelHandle.Clear();
            }

            this->_currentLevelHandle = this->_nextLevelHandle;
            this->_nextLevelHandle.Clear();
        }

        this->_upNodeManager->BeginUpdate(in_fDt);
    }

    void LevelModule::_VUpdate(const HE::Float32 in_fDt)
    {
        this->_upNodeManager->Update(in_fDt);
    }

    void LevelModule::_VLateUpdate(const HE::Float32 in_fDt)
    {
        this->_upNodeManager->LateUpdate(in_fDt);
    }

    HE::Bool LevelModule::_InitMainLevel(const Core::Common::Handle& in_rHandle)
    {
        this->_nextLevelHandle = in_rHandle;

        auto pNode = reinterpret_cast<Node*>(this->_upNodeManager->Get(this->_nextLevelHandle));
        HE_ASSERT(pNode);
        if (pNode == NULL) return FALSE;

        // 切り替えた後にアクティブにする
        pNode->SetState(Actor::Object::EState::EState_Paused);

        return TRUE;
    }

}  // namespace Level
