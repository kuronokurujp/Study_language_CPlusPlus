#include "DXLibTime.h"

#include "DxLib.h"

namespace DXLib
{
    /// <summary>
    /// 現在の経過時間を取得(ミリセコンド)
    /// </summary>
    /// <returns></returns>
    Uint64 Time::VNowMSec()
    {
        return static_cast<Uint64>(GetNowCount());
    }

    /// <summary>
    /// 指定した時間停止(ミリセコンド)
    /// </summary>
    void Time::VSleepMSec(const Uint64 in_uMs)
    {
        DxLib::SleepThread(static_cast<int>(in_uMs));
    }
}  // namespace DXLib
