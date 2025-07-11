#include "Engine/Platform/Graphic/FrameBase.h"

namespace Platform
{
    FrameBase::FrameBase(const FrameConfig& in_rConfig,
                         Core::Memory::UniquePtr<EventFrameInterface> in_upEvent)
        : _config(in_rConfig), _upEvent(std::move(in_upEvent))
    {
    }

}  // namespace Platform
