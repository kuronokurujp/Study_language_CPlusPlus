#include "PlatformSDL2/SDL2System.h"

#include <time.h>

#include <random>

// SDL2のヘッダーファイル
#include "SDL2/SDL.h"

namespace PlatformSDL2
{
    namespace Local
    {
        static std::mt19937 rng;
    }  // namespace Local

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

    HE::Float32 System::VGetRandByFloat(const HE::Float32 in_fMin, const HE::Float32 in_fMax)
    {
        // 一様実数分布
        std::uniform_real_distribution uniform(in_fMin, in_fMax);
        return uniform(Local::rng);
    }

    HE::Bool System::VSetSeedRandByFloat()
    {
        Local::rng = std::mt19937(::SDL_GetTicks());
        return TRUE;
    }

}  // namespace PlatformSDL2
