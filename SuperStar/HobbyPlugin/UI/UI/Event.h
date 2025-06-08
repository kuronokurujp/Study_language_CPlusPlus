#pragma once

// エンジン最小インクルード
#include "Engine/Common/Hash.h"
#include "Engine/MiniEngine.h"

// イベントのモジュール
#include "EventModule.h"

#define EVENT_TYPE_UIMODULE (Event::BaseEventData::ListenerSpecal)

namespace UI
{
    /// <summary>
    /// UIボタンをクリックイベント
    /// </summary>
    class EventButtonClick final : public Event::BaseEventData
    {
    public:
        EventButtonClick(const HE::UTF8* in_pMsg)
            : Event::BaseEventData(s_szEventName), _szMsg(in_pMsg)
        {
        }

        static inline HE::Uint32 Hash() { return s_uEventHash; }

    public:
        Core::Common::FixedString32 _szMsg;

    private:
        static inline Event::EventTypeStr s_szEventName = HE_STR_TEXT("UIEvent_ButtonClick");
        static inline HE::Uint32 s_uEventHash           = s_szEventName.Hash();
    };

}  // namespace UI
