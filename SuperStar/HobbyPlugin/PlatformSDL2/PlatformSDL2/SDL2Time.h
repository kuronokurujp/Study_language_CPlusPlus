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
        Uint64 VNowMSec() override final;
        void VSleepMSec(const Uint64 in_uMs) override final;
    };
}  // namespace Platform::SDL2
