#include "EventModule.h"

namespace Event
{
    EventModule::EventModule() : ModuleBase(ModuleName())
    {
    }

    const Core::Common::Handle EventModule::AddListener(EventListenerPtr const& in_rListener,
                                                        const HE::Hash in_hash)
    {
        if (this->_pEventSingleProcess == NULL)
        {
            this->_pEventSingleProcess = HE_NEW_MEM(EventProcess, 0)(HE_STR_TEXT("Single"));
        }

        return this->_pEventSingleProcess->AddListener(in_rListener, in_hash);
    }

    HE::Bool EventModule::RemoveListener(const HE::Hash in_ulHashistener)
    {
        return this->_pEventSingleProcess->RemoveListener(in_ulHashistener);
    }

    HE::Bool EventModule::RemoveAllListener()
    {
        return this->_pEventSingleProcess->RemoveAllListener();
    }

    HE::Bool EventModule::QueueEvent(EventDataInterfacePtr const& in_spEventData,
                                     const HE::Hash in_hash)
    {
        return this->_pEventSingleProcess->QueueEvent(in_spEventData, in_hash);
    }

    /// <summary>
    /// モジュール初期化
    /// </summary>
    HE::Bool EventModule::_VStart()
    {
        if (this->_pEventSingleProcess == NULL)
        {
            this->_pEventSingleProcess = HE_NEW_MEM(EventProcess, 0)(HE_STR_TEXT("Single"));
        }

        return TRUE;
    }

    /// <summary>
    /// インスタンス破棄時に呼ばれる
    /// </summary>
    HE::Bool EventModule::_VRelease()
    {
        // 全ての管理インスタンスを破棄
        this->_pEventSingleProcess->Release();
        HE_SAFE_DELETE_MEM(this->_pEventSingleProcess);

        return TRUE;
    }

    void EventModule::_VLateUpdate(const HE::Float32 in_fDeltaTime)
    {
        this->_pEventSingleProcess->Tick(EventProcess::EConstancs_Infinite);
    }

}  // namespace Event
