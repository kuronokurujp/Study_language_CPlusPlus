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
        EventModule();

        /// <summary>
        /// リスナー登録 / 解除
        /// 登録したらTRUEを返す
        /// すでに登録済みなど登録失敗したらFALSE
        /// </summary>
        const Core::Common::Handle AddListener(EventListenerPtr const&, const HE::Hash);
        HE::Bool RemoveListener(const HE::Hash);
        // リスナーを一括で破棄
        HE::Bool RemoveAllListener();

        /// <summary>
        /// 実行したいイベントを設定
        /// </summary>
        HE::Bool QueueEvent(EventDataInterfacePtr const&, const HE::Hash);

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
        EventProcess* _pEventSingleProcess = NULL;
    };

}  // namespace Event
