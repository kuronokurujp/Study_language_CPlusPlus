#pragma once

#include "Engine/Core.h"

// プラットフォームのシステム周りのインターフェイス

namespace Platform
{
    class SystemInterface
    {
    public:
        virtual ~SystemInterface()                      = default;
        virtual HE::Uint32 VGetRand(const HE::Uint32)   = 0;
        virtual HE::Bool VSetSeedRand(const HE::Uint32) = 0;
    };

}  // namespace Platform
