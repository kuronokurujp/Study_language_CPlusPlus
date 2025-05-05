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

        // virtual const EventTypeStr& VEventTypeStr() const = 0;
        virtual HE::Uint64 VNetworkHash() const = 0;
        virtual HE::Uint64 VEventHash() const   = 0;
        virtual HE::Float32 VTimeStamp() const  = 0;
        // virtual void VSerialize(std::ostrstream& out) const = 0;
    };

    // イベントデータのポインタ
    using EventDataInterfacePtr = Core::Memory::SharedPtr<EventDataInterface>;

    class BaseEventData : public EventDataInterface
    {
    public:
        enum EType
        {
            EType_SpecalEvent = 0,
        };

        explicit BaseEventData(const EventTypeStr& in_rNetworkName,
                               const EventTypeStr& in_rDataType,
                               const HE::Float32 in_fTimeStamp = 0.0f)
            : _fTimeStamp(in_fTimeStamp),
              _ulNetworkHash(in_rNetworkName.Hash()),
              _ulEventHash(in_rDataType.Hash())
        {
        }

        virtual ~BaseEventData() = default;

        HE::Uint64 VNetworkHash() const override final { return this->_ulNetworkHash; }
        HE::Uint64 VEventHash() const override final { return this->_ulEventHash; }
        HE::Float32 VTimeStamp() const override final { return this->_fTimeStamp; }

    protected:
        const HE::Float32 _fTimeStamp   = 0.0f;
        const HE::Uint64 _ulNetworkHash = 0;
        const HE::Uint64 _ulEventHash   = 0;
    };
}  // namespace Event
