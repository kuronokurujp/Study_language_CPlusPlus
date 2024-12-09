#pragma once

#include "Engine/Platform/PlatformSystem.h"

namespace PlatformSDL2
{
    /// <summary>
    /// DXLibプラットフォームのシステム
    /// </summary>
    class System final : public Platform::SystemInterface
    {
    public:
        HE::Uint32 VGetRand(const HE::Uint32) override final;
        HE::Bool VSetSeedRand(const HE::Uint32) override final;
    };
}  // namespace PlatformSDL2
