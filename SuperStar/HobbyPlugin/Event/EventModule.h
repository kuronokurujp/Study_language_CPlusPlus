#pragma once

#include "Engine/MiniEngine.h"
#include "Engine/Module/Module.h"

// モジュールのヘッダーファイルは全てインクルードする
#include "Event/EventManager.h"

namespace Event
{
    /// <summary>
    /// イベント用の追加モジュール
    /// </summary>
    class EventModule final : public Module::ModuleBase
    {
        HE_MODULE_GENRATE_DECLARATION(EventModule);

    public:
        EventModule() : ModuleBase(ModuleName()) {}

        const Core::Common::Handle AddEventManager(
            Core::Memory::UniquePtr<EventManagerStrategyInterface>);
        HE::Bool RemoveEventManager(const Core::Common::Handle&);

        /// <summary>
        /// リスナー登録 / 解除
        /// 登録したらTRUEを返す
        /// すでに登録済みなど登録失敗したらFALSE
        /// </summary>
        HE::Bool AddListener(EventListenerPtr const&, EventTypeStr const&);
        HE::Bool RemoveListener(EventListenerPtr const&, EventTypeStr const&);
        // 指定タイプのリスナーを一括で破棄
        HE::Bool RemoveAllListener(EventTypeStr const&);

        /// <summary>
        /// 実行したいイベントを設定
        /// </summary>
        HE::Bool QueueEvent(EventDataInterfacePtr const&);

    protected:
        /// <summary>
        /// モジュール初期化
        /// </summary>
        HE::Bool _VStart() override final;

        /// <summary>
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        HE::Bool _VRelease() override final;

        /// <summary>
        /// モジュール後更新
        /// </summary>
        void _VLateUpdate(const HE::Float32 in_fDeltaTime) override final;

    private:
        HE::Uint32 _uAddEventMngCount = 0;
        Core::Common::FixedMap<Core::Common::Handle, EventManager*, 32> _mEventMng;
    };

}  // namespace Event
