#pragma once

#include "Engine/Common/Hash.h"
#include "Engine/Math/Vector2.h"
#include "InGame/InGameTag.h"
// エンジン最小インクルード
#include "Engine/MiniEngine.h"

// イベントのモジュール
#include "EventModule.h"

#define INGAME_CHARACTER_EVENT_TYPE_NAME HE_STR_TEXT("InGameCharacterEvent")

namespace InGame
{
    class InGameBulletManagerComponent;

    /// <summary>
    /// キャラクターのイベント管理の拡張クラス
    /// </summary>
    class InGameCharacterEventManagerStrategy final : public Event::EventManagerStrategyInterface
    {
    public:
        InGameCharacterEventManagerStrategy()
            : _ulEventTypeHash(Core::Common::HashName(INGAME_CHARACTER_EVENT_TYPE_NAME))
        {
        }

        Bool VIsEventTypeHash(const Uint64 in_ulHash)
        {
            return (this->_ulEventTypeHash == in_ulHash);
        }

    private:
        Uint64 _ulEventTypeHash = 0;
    };

    /// <summary>
    /// 移動
    /// </summary>
    class EventCharacterMove final : public Event::BaseEventData
    {
    public:
        EventCharacterMove(const Float32 in_fTimeStamp, const EObjectTag in_eTag,
                           const Uint32 in_uId, const Core::Math::Vector2& in_rMove);

        static inline Uint32 EventTypeHash() { return s_uEventTypeHash; }

    public:
        Core::Math::Vector2 _move;
        EObjectTag _eTag = EObjectTag::EObjectTag_None;
        Uint32 _uId      = 0;

    private:
        static inline Event::EventTypeStr s_szEventType = HE_STR_TEXT("CharacterMove");
        static inline Uint32 s_uEventTypeHash           = s_szEventType.Hash();
    };

    /// <summary>
    /// 攻撃
    /// </summary>
    class EventCharacterAttack final : public Event::BaseEventData
    {
    public:
        EventCharacterAttack(const Float32 in_fTimeStamp, const EObjectTag in_eTag,
                             const Uint32 in_uId);

        static inline Uint32 EventTypeHash() { return s_uEventTypeHash; }

    public:
        EObjectTag _eTag = EObjectTag::EObjectTag_None;
        Uint32 _uId      = 0;

    private:
        static inline Event::EventTypeStr s_szEventType = HE_STR_TEXT("CharacterAttack");
        static inline Uint32 s_uEventTypeHash           = s_szEventType.Hash();
    };

    /// <summary>
    /// 敵生成
    /// </summary>
    class EventCharacterPutEnemy final : public Event::BaseEventData
    {
    public:
        EventCharacterPutEnemy(const Float32 in_fTimeStamp, const Core::Math::Vector2& in_rPos,
                               const EEnemyTag in_eTag, const Uint32 in_uId,
                               const UTF8* in_szIdName);

        static inline Uint32 EventTypeHash() { return s_uEventTypeHash; }

    public:
        Core::Math::Vector2 _pos;
        EEnemyTag _eEnemyTag = EEnemyTag::EEnemyTag_None;
        Uint32 _uId          = 0;
        Core::Common::FixString32 _szIdName;

    private:
        static inline Event::EventTypeStr s_szEventType = HE_STR_TEXT("CharacterPutEnemy");
        static inline Uint32 s_uEventTypeHash           = s_szEventType.Hash();
    };

    /// <summary>
    /// 自機生成
    /// </summary>
    class EventCharacterPutPlayer final : public Event::BaseEventData
    {
    public:
        EventCharacterPutPlayer(const Float32 in_fTimeStamp, const Core::Math::Vector2& in_rPos);

        static inline Uint32 EventTypeHash() { return s_uEventTypeHash; }

    public:
        Core::Math::Vector2 _pos;

    private:
        static inline Event::EventTypeStr s_szEventType = HE_STR_TEXT("CharacterPutPlayer");
        static inline Uint32 s_uEventTypeHash           = s_szEventType.Hash();
    };

    /// <summary>
    /// ダメージ通知
    /// </summary>
    class EventCharacterDamage final : public Event::BaseEventData
    {
    public:
        EventCharacterDamage(const Float32 in_fTimeStamp, EObjectTag in_eObjectTag,
                             const Uint64 in_ulHandle, const Sint32 in_sDamage);

        static inline Uint32 EventTypeHash() { return s_uEventTypeHash; }

    public:
        Sint32 _sDamage        = 0;
        EObjectTag _eObjectTag = EObjectTag::EObjectTag_None;
        Uint64 _ulHandle       = 0;

    private:
        static inline Event::EventTypeStr s_szEventType = HE_STR_TEXT("CharacterDamage");
        static inline Uint32 s_uEventTypeHash           = s_szEventType.Hash();
    };
}  // namespace InGame
