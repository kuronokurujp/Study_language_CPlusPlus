#pragma once

#include "Engine/Common/Hash.h"
#include "Engine/Math/Vector2.h"
// エンジン最小インクルード
#include "Engine/MiniEngine.h"

// イベントのモジュール
#include "EventModule.h"

#define EVENT_TYPE_INGAME_SHOT (Core::Common::HashName(HE_STR_TEXT("InGameShotEvent")))

namespace InGame
{
    class InGameBulletManagerComponent;

    /// <summary>
    /// 通常弾を発射するイベント
    /// </summary>
    class EventShotNormalBullet final : public Event::BaseEventData
    {
    public:
        EventShotNormalBullet(const HE::Float32 in_fTimeStamp, const Core::Math::Vector2& in_rPos,
                              const Core::Math::Vector2& in_rDir,
                              const HE::Uint32 in_uCollisionHashCode);

        static HE::Uint32 EventTypeHash();

    public:
        Core::Math::Vector2 _pos;
        Core::Math::Vector2 _dir;
        HE::Uint32 _uCollisionHashCode = 0;

    private:
        static inline Event::EventTypeStr s_szEventType = HE_STR_TEXT("ShotNormalBullet");
        static inline HE::Uint32 s_uEventTypeHash       = s_szEventType.Hash();
    };
}  // namespace InGame
