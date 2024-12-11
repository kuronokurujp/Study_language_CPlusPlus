#include "SDL2System.h"

#include <time.h>

namespace PlatformSDL2
{
    HE::Uint32 System::VGetRand(const HE::Uint32 in_uMax)
    {
        // 0からin_uMax間の値を取得
        return static_cast<HE::Uint32>(rand() % (in_uMax + 1));
    }

    HE::Bool System::VSetSeedRand(const HE::Uint32 in_uVal)
    {
        // 乱数の種を初期化
        srand(static_cast<unsigned int>(in_uVal));

        return TRUE;
    }
}  // namespace PlatformSDL2
