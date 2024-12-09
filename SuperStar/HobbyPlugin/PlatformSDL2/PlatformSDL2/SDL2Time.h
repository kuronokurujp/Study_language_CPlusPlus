#pragma once

#include "Engine/Platform/PlatformTime.h"

namespace PlatformSDL2
{
    /// <summary>
    /// プラットフォームの時間インターフェイス
    /// </summary>
    class Time final : public Platform::TimeInterface
    {
    public:
        HE::Uint64 VNowMSec() override final;
        void VSleepMSec(const HE::Uint64 in_uMs) override final;
    };
}  // namespace PlatformSDL2
