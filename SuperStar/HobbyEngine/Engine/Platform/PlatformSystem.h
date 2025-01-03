#pragma once

#include "Engine/Core.h"

// プラットフォームのシステム周りのインターフェイス

namespace Platform
{
    class SystemInterface
    {
    public:
        virtual ~SystemInterface() = default;
        // 整数のみの乱数
        virtual HE::Uint32 VGetRand(const HE::Uint32)   = 0;
        virtual HE::Bool VSetSeedRand(const HE::Uint32) = 0;

        // 小数点を含めた乱数
        virtual HE::Float32 VGetRandByFloat(const HE::Float32 in_fMin,
                                            const HE::Float32 in_fMax) = 0;
        virtual HE::Bool VSetSeedRandByFloat()                         = 0;
    };

}  // namespace Platform
