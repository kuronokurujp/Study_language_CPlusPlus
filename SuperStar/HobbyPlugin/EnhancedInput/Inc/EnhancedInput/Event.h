#pragma once

// エンジン最小インクルード
#include "Engine/Common/Hash.h"
#include "Engine/MiniEngine.h"
#include "Engine/Platform/PlatformInput.h"

// イベントのモジュール
#include "EventModule.h"

// モジュール内のファイル
#include "EnhancedInput/Common.h"

// 拡張インプットのイベントタイプハッシュ値
#define EVENT_TYPE_ENHANCEDINPUT (Event::BaseEventData::ListenerSpecal)

namespace EnhancedInput
{
    /// <summary>
    /// 入力状態イベント
    /// </summary>
    class EventInput final : public Event::BaseEventData
    {
    public:
        EventInput() : Event::BaseEventData(s_szEventName) {}

        const HE::Bool Empty() const { return this->_mInputAction.Empty(); }

        static inline HE::Uint32 Hash() { return s_uEventHash; }

    public:
        /// <summary>
        /// マッピングしたアクションの入力リスト
        /// </summary>
        InputMap _mInputAction;

    private:
        static inline Event::EventTypeStr s_szEventName = HE_STR_TEXT("EnhancedInputEvent_Input");
        static inline HE::Uint32 s_uEventHash           = s_szEventName.Hash();
    };

}  // namespace EnhancedInput
