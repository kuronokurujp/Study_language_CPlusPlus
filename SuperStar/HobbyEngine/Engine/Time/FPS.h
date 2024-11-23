#pragma once

#include "Engine/Core.h"

// 前方宣言
namespace Platform
{
    class TimeInterface;
}

/// <summary>
/// ゲーム中のFPS制御
/// </summary>
namespace Core::Time
{
    class FPS
    {
    public:
        FPS() = default;
        FPS(Platform::TimeInterface* in_pTimeInterface);

        /// <summary>
        /// 指定時間待機(ミリ秒)
        /// </summary>
        Bool UpdateWait(Platform::TimeInterface* in_pTimeInterface, const Uint32 in_uWaitMSec);

        /// <summary>
        /// 更新時間取得(秒)
        /// </summary>
        Float32 GetDeltaTimeSec() const;
        Float32 GetDeltaTimeMSec() const;

        inline Uint32 GetFrameRate() const { return this->_uFrameRate; }

    private:
        static inline const Uint32 _uTimeAvgCount    = 10;
        static inline const Float32 _fFrameAvgTimeMSec = 1000.0f * _uTimeAvgCount;

        Uint32 _uaPreviousTimeMSec[_uTimeAvgCount] = {0};
        Float32 _uFrameRate                  = 0;
        Uint32 _uCurrentTime                 = 0;
    };
}  // namespace Core::Time
