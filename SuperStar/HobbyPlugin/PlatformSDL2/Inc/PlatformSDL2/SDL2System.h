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

        // 小数点を含めた乱数
        HE::Float32 VGetRandByFloat(const HE::Float32 in_fMin,
                                    const HE::Float32 in_fMax) override final;
        HE::Bool VSetSeedRandByFloat() override final;
    };
}  // namespace PlatformSDL2
