#pragma once

#include "Engine/Common/CustomString.h"
#include "Engine/MiniEngine.h"

namespace Event
{
    using EventTypeStr = Core::Common::FixedString128;

    class EventDataInterface
    {
    public:
        virtual ~EventDataInterface() = default;

        // virtual const EventTypeStr& VEventTypeName() const = 0;
        virtual HE::Uint64 VEventHash() const          = 0;
        virtual HE::Float32 VTimeStamp() const         = 0;
        virtual const HE::Char* VEventTypeName() const = 0;
        // virtual void VSerialize(std::ostrstream& out) const = 0;
    };

    // イベントデータのポインタ
    using EventDataInterfacePtr = Core::Memory::SharedPtr<EventDataInterface>;

    class BaseEventData : public EventDataInterface
    {
    public:
        static const HE::Hash ListenerSpecal = 0;

        explicit BaseEventData(const EventTypeStr& in_rDataType,
                               const HE::Float32 in_fTimeStamp = 0.0f)
            : _fTimeStamp(in_fTimeStamp),
              _ulEventHash(in_rDataType.Hash())
#ifdef HE_ENGINE_DEBUG
              ,
              _szDataType(in_rDataType)
#endif  // HE_DEBUG

        {
        }

        virtual ~BaseEventData() = default;

        HE::Uint64 VEventHash() const override final { return this->_ulEventHash; }
        HE::Float32 VTimeStamp() const override final { return this->_fTimeStamp; }

        const HE::Char* VEventTypeName() const override final
        {
#ifdef HE_ENGINE_DEBUG
            return this->_szDataType.Str();
#else
            return HE_STR_EMPTY;
#endif
        }

    protected:
        const HE::Float32 _fTimeStamp = 0.0f;
        const HE::Uint64 _ulEventHash = 0;
#ifdef HE_ENGINE_DEBUG
        const EventTypeStr _szDataType;
#endif  // HE_DEBUG
    };
}  // namespace Event
