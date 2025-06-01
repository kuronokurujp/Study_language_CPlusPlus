#include "InGameEventCharacter.h"

namespace InGame
{
    EventCharacterMove::EventCharacterMove(const HE::Float32 in_fTimeStamp,
                                           const EObjectTag in_eTag, const HE::Uint64 in_uuHandle,
                                           const Core::Math::Vector2& in_rMove)
        : Event::BaseEventData(s_szEventType, in_fTimeStamp),
          _move(in_rMove),
          _eTag(in_eTag),
          _uuHandle(in_uuHandle)
    {
    }

    EventCharacterAttack::EventCharacterAttack(const HE::Float32 in_fTimeStamp,
                                               const EObjectTag in_eTag,
                                               const HE::Uint64 in_uuHandle)
        : Event::BaseEventData(s_szEventType, in_fTimeStamp), _eTag(in_eTag), _uuHandle(in_uuHandle)

    {
    }

    EventCharacterPutEnemy::EventCharacterPutEnemy(const HE::Float32 in_fTimeStamp,
                                                   const Core::Math::Vector2& in_rPos,
                                                   const EEnemyTag in_eTag, const HE::Uint32 in_uId,
                                                   const HE::UTF8* in_szIdName)
        : Event::BaseEventData(s_szEventType, in_fTimeStamp),
          _pos(in_rPos),
          _eEnemyTag(in_eTag),
          _uId(in_uId),
          _szIdName(in_szIdName)
    {
    }

    EventCharacterPutPlayer::EventCharacterPutPlayer(const HE::Float32 in_fTimeStamp,
                                                     const Core::Math::Vector2& in_rPos)
        : Event::BaseEventData(s_szEventType, in_fTimeStamp), _pos(in_rPos)
    {
    }

    EventCharacterDamage::EventCharacterDamage(const HE::Float32 in_fTimeStamp,
                                               EObjectTag in_eObjectTag,
                                               const HE::Uint64 in_ulHandle,
                                               const HE::Uint32 in_sDamage)
        : Event::BaseEventData(s_szEventType, in_fTimeStamp),
          _ulHandle(in_ulHandle),
          _eObjectTag(in_eObjectTag),
          _sDamage(in_sDamage)

    {
    }

}  // namespace InGame
