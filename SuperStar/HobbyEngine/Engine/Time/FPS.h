#pragma once

#include "Engine/Core.h"
#include "Engine/Memory/Memory.h"

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
        HE_CLASS_DEFAULT_CONSTRUCT_NG(FPS);

    public:
        FPS(Core::Memory::WeakPtr<Platform::TimeInterface>);

        /// <summary>
        /// 指定時間待機(ミリ秒)
        /// </summary>
        HE::Bool UpdateWait(Core::Memory::WeakPtr<Platform::TimeInterface> in_wpTimeInterface,
                        const HE::Uint32 in_uWaitMSec);

        /// <summary>
        /// 更新時間取得(秒)
        /// </summary>
        HE::Float32 GetDeltaTimeSec() const;
        HE::Float32 GetDeltaTimeMSec() const;

        inline HE::Uint32 GetFrameRate() const { return this->_uFrameRate; }

    private:
        static inline const HE::Uint32 _uTimeAvgCount      = 10;
        static inline const HE::Float32 _fFrameAvgTimeMSec = 1000.0f * _uTimeAvgCount;

        HE::Uint32 _uaPreviousTimeMSec[_uTimeAvgCount] = {0};
        HE::Float32 _uFrameRate                        = 0;
        HE::Uint32 _uCurrentTime                       = 0;
    };
}  // namespace Core::Time
