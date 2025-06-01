#pragma once

#include "Engine/Common/Hash.h"
#include "Engine/Math/Vector2.h"
#include "InGame/InGameTag.h"
// エンジン最小インクルード
#include "Engine/MiniEngine.h"

// イベントのモジュール
#include "EventModule.h"

#define EVENT_TYPE_INGAME_CHARACTER (Core::Common::HashName(HE_STR_TEXT("InGameCharacterEvent")))

namespace InGame
{
    class InGameBulletManagerComponent;

    /// <summary>
    /// 移動
    /// </summary>
    class EventCharacterMove final : public Event::BaseEventData
    {
    public:
        EventCharacterMove(const HE::Float32 in_fTimeStamp, const EObjectTag in_eTag,
                           const HE::Uint64 in_uuHandle, const Core::Math::Vector2& in_rMove);

        static inline HE::Uint32 EventTypeHash() { return s_uEventTypeHash; }

    public:
        Core::Math::Vector2 _move;
        HE::Uint64 _uuHandle = 0;
        EObjectTag _eTag     = EObjectTag::EObjectTag_None;

    private:
        static inline Event::EventTypeStr s_szEventType = HE_STR_TEXT("CharacterMove");
        static inline HE::Uint32 s_uEventTypeHash       = s_szEventType.Hash();
    };

    /// <summary>
    /// 攻撃
    /// </summary>
    class EventCharacterAttack final : public Event::BaseEventData
    {
    public:
        EventCharacterAttack(const HE::Float32 in_fTimeStamp, const EObjectTag in_eTag,
                             const HE::Uint64 in_uuHandle);

        static inline HE::Uint32 EventTypeHash() { return s_uEventTypeHash; }

    public:
        EObjectTag _eTag     = EObjectTag::EObjectTag_None;
        HE::Uint64 _uuHandle = 0;

    private:
        static inline Event::EventTypeStr s_szEventType = HE_STR_TEXT("CharacterAttack");
        static inline HE::Uint32 s_uEventTypeHash       = s_szEventType.Hash();
    };

    /// <summary>
    /// 敵生成
    /// </summary>
    class EventCharacterPutEnemy final : public Event::BaseEventData
    {
    public:
        EventCharacterPutEnemy(const HE::Float32 in_fTimeStamp, const Core::Math::Vector2& in_rPos,
                               const EEnemyTag in_eTag, const HE::Uint32 in_uId,
                               const HE::UTF8* in_szIdName);

        static inline HE::Uint32 EventTypeHash() { return s_uEventTypeHash; }

    public:
        Core::Math::Vector2 _pos;
        EEnemyTag _eEnemyTag = EEnemyTag::EEnemyTag_None;
        HE::Uint32 _uId      = 0;
        Core::Common::FixedString32 _szIdName;

    private:
        static inline Event::EventTypeStr s_szEventType = HE_STR_TEXT("CharacterPutEnemy");
        static inline HE::Uint32 s_uEventTypeHash       = s_szEventType.Hash();
    };

    /// <summary>
    /// 自機生成
    /// </summary>
    class EventCharacterPutPlayer final : public Event::BaseEventData
    {
    public:
        EventCharacterPutPlayer(const HE::Float32 in_fTimeStamp,
                                const Core::Math::Vector2& in_rPos);

        static inline HE::Uint32 EventTypeHash() { return s_uEventTypeHash; }

    public:
        Core::Math::Vector2 _pos;

    private:
        static inline Event::EventTypeStr s_szEventType = HE_STR_TEXT("CharacterPutPlayer");
        static inline HE::Uint32 s_uEventTypeHash       = s_szEventType.Hash();
    };

    /// <summary>
    /// ダメージ通知
    /// </summary>
    class EventCharacterDamage final : public Event::BaseEventData
    {
    public:
        EventCharacterDamage(const HE::Float32 in_fTimeStamp, EObjectTag in_eObjectTag,
                             const HE::Uint64 in_ulHandle, const HE::Uint32 in_sDamage);

        static inline HE::Uint32 EventTypeHash() { return s_uEventTypeHash; }

    public:
        HE::Uint32 _sDamage    = 0;
        EObjectTag _eObjectTag = EObjectTag::EObjectTag_None;
        HE::Uint64 _ulHandle   = 0;

    private:
        static inline Event::EventTypeStr s_szEventType = HE_STR_TEXT("CharacterDamage");
        static inline HE::Uint32 s_uEventTypeHash       = s_szEventType.Hash();
    };
}  // namespace InGame
