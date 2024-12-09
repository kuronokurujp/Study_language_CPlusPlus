#include "SDL2Time.h"

namespace PlatformSDL2
{
    /// <summary>
    /// 現在の経過時間を取得(ミリセコンド)
    /// </summary>
    /// <returns></returns>
    HE::Uint64 Time::VNowMSec()
    {
        return 0;
        // return static_cast<Uint64>(GetNowCount());
    }

    /// <summary>
    /// 指定した時間停止(ミリセコンド)
    /// </summary>
    void Time::VSleepMSec(const HE::Uint64 in_uMs)
    {
        // DxLib::SleepThread(static_cast<int>(in_uMs));
    }
}  // namespace PlatformSDL2
