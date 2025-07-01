#include "PlatformSDL2/SDL2Time.h"

// SDL2のヘッダーファイル
#include "SDL2/SDL.h"

namespace PlatformSDL2
{
    /// <summary>
    /// 現在の経過時間を取得(ミリセコンド)
    /// </summary>
    HE::Uint64 Time::VNowMSec()
    {
        return ::SDL_GetTicks();
    }

    /// <summary>
    /// 指定した時間停止(ミリセコンド)
    /// </summary>
    void Time::VSleepMSec(const HE::Uint64 in_uMs)
    {
        ::SDL_Delay(in_uMs);
    }
}  // namespace PlatformSDL2
