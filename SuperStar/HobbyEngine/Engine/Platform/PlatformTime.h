#pragma once

#include "Engine/Core.h"

// プラットフォームの時間周りのインターフェイス

namespace Platform
{
    class TimeInterface
    {
    public:
        virtual ~TimeInterface() = default;

        virtual Uint64 VNowMSec()                   = 0;
        virtual void VSleepMSec(const Uint64 in_ms) = 0;
    };

}  // namespace Platform
